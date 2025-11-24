#include "codipch.h"
#include "VulkanShader.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanGlobalUniformRegistry.h"

#include <fstream>
#include <spirv_cross/spirv_cross.hpp>

namespace Codi {

    namespace Utils {

        static VkFormat ShaderDataTypeToVkFormat(ShaderDataType type) {
            switch (type) {
            case ShaderDataType::Float:  return VK_FORMAT_R32_SFLOAT;
            case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
            case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
            case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
            case ShaderDataType::Int:    return VK_FORMAT_R32_SINT;
            case ShaderDataType::Int2:   return VK_FORMAT_R32G32_SINT;
            case ShaderDataType::Int3:   return VK_FORMAT_R32G32B32_SINT;
            case ShaderDataType::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
            case ShaderDataType::Bool:   return VK_FORMAT_R8_UINT;
            default:
                CODI_CORE_ASSERT(false, "Unknown ShaderDataType");
                return VK_FORMAT_UNDEFINED;
            }
        }

        static shaderc_shader_kind ShaderTypeToShaderC(Shader::Type stage) {
            switch (stage) {
            case Shader::Type::Vertex: return shaderc_glsl_vertex_shader;
            case Shader::Type::Fragment: return shaderc_glsl_fragment_shader;
            }
            CODI_CORE_ASSERT(false, "Unknown shader stage");
            return shaderc_glsl_vertex_shader;
        }

        static VkShaderStageFlagBits ShaderTypeToVkStage(Shader::Type stage) {
            switch (stage) {
            case Shader::Type::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
            case Shader::Type::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
            }
            CODI_CORE_ASSERT(false, "Unknown shader stage");
            return VK_SHADER_STAGE_VERTEX_BIT;
        }

        static const char* GetCacheDirectory() {
            // TODO: check assets directory validity
            return "Resources/cache/shader/vulkan";
        }

        static void CreateCacheDirectoryIfNeeded() {
            std::string cacheDirectory = GetCacheDirectory();
            if (!std::filesystem::exists(cacheDirectory))
                std::filesystem::create_directories(cacheDirectory);
        }

        static const char* GLShaderStageCachedVulkanFileExtension(Shader::Type stage) {
            switch (stage) {
            case Shader::Type::Vertex: return ".cached_vulkan.vert";
            case Shader::Type::Fragment: return ".cached_vulkan.frag";
            }
            CODI_CORE_ASSERT(false, "Unknown shader stage");
            return "";
        }

    } // namespace Utils

    VulkanShader::VulkanShader(const std::filesystem::path& filepath) : _Filepath(filepath) {
        std::string raw = ReadFile(filepath);
        std::unordered_map<Shader::Type, std::string> shaderSources = Preprocess(raw);

        CompileOrGetBinaries(shaderSources);

        for (auto& [type, stage] : _Stages) {
            if (stage.SPIRV.empty()) continue;
            stage.Module = CreateModule(stage.SPIRV);
        }

        // Extract name from filepath
        _Name = filepath.stem().string();

        CreateDescriptorSetLayouts();
        CreateDescriptorPoolAndSets();
        CreateUniformBuffers();
    }

    VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : _Name(name), _Filepath(name) {
        std::unordered_map<Shader::Type, std::string> shaderSources = {
            { Shader::Type::Vertex, vertexSrc, },
            { Shader::Type::Fragment, fragmentSrc, }
        };

        CompileOrGetBinaries(shaderSources);

        for (auto& [type, stage] : _Stages) {
            if (stage.SPIRV.empty()) continue;
            stage.Module = CreateModule(stage.SPIRV);
        }

        CreateDescriptorSetLayouts();
        CreateDescriptorPoolAndSets();
        CreateUniformBuffers();
    }

    VulkanShader::~VulkanShader() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        if (_Pipeline) {
            _Pipeline->Destroy();
            _Pipeline = nullptr;
        }

        for (auto& [stage, data] : _Stages)
            vkDestroyShaderModule(logicalDevice, data.Module, VulkanRendererAPI::GetAllocator());
        _Stages.clear();

        if (_DescriptorPool) {
            vkDestroyDescriptorPool(logicalDevice, _DescriptorPool, VulkanRendererAPI::GetAllocator());
            _DescriptorPool = VK_NULL_HANDLE;
        }
        for (auto& layout : _DescriptorSetLayouts)
            if (layout)
                vkDestroyDescriptorSetLayout(logicalDevice, layout, VulkanRendererAPI::GetAllocator());
        _DescriptorSetLayouts.clear();

        // Destroy uniform buffers
        DestroyUniformBuffers();
    }

    void VulkanShader::Bind() {
        if (Renderer::IsFrameSkipped()) return;

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VulkanGraphicsContext* context = api.GetContext();
        uint32 imageIndex = api.GetCurrentFrameIndex();
        VulkanCommandBuffer* cmdBuffer = api.GetCommandBuffer(imageIndex);

        _Pipeline->Bind(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);

        std::vector<VkDescriptorSet> boundSets;
        boundSets.reserve(_DescriptorSetLayouts.size());

        for (uint32 s = 0; s < _DescriptorSets.size(); s++) {
            if (_DescriptorSets[s].empty()) continue;
            boundSets.push_back(_DescriptorSets[s][imageIndex % (uint32)_DescriptorSets[s].size()]);
        }

        if (!boundSets.empty()) {
            vkCmdBindDescriptorSets(
                cmdBuffer->GetHandle(),
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                _Pipeline->GetLayout(),
                0,
                (uint32)boundSets.size(),
                boundSets.data(),
                0,
                nullptr
            );
        }
    }

    void VulkanShader::CreatePipeline(Shared<VertexBuffer> quadVertexBuffer) {
        // Attributes
        const BufferLayout& layout = quadVertexBuffer->GetLayout();
        const std::vector<BufferElement>& layoutElements = layout.GetElements();
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        for (uint32 i = 0; i < layoutElements.size(); i++) {
            const BufferElement& element = layoutElements[i];

            VkVertexInputAttributeDescription desc{};
            desc.binding = 0;
            desc.location = i;
            desc.format = Utils::ShaderDataTypeToVkFormat(element.Type);
            desc.offset = element.Offset;
            attributeDescriptions.push_back(desc);
        }

        // Stages
        std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfos;
        stageCreateInfos.reserve(_Stages.size());
        for (auto& [type, stage] : _Stages) {
            if (stage.Module == VK_NULL_HANDLE) continue;

            VkPipelineShaderStageCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            createInfo.stage = Utils::ShaderTypeToVkStage(type);
            createInfo.module = stage.Module;
            createInfo.pName = "main";

            stageCreateInfos.push_back(createInfo);
        }

        _Pipeline = Share<VulkanPipeline>();
        _Pipeline->Create(attributeDescriptions, _DescriptorSetLayouts, stageCreateInfos, layout.GetStride(), false);
    }

    std::string VulkanShader::ReadFile(const std::filesystem::path& filepath) {
        std::ifstream infile(filepath, std::ios::binary);
        if (!infile) {
            CODI_CORE_ERROR("Could not open file '{0}'", filepath.string());
            return "";
        }

        std::string result;
        infile.seekg(0, std::ios::end);
        result.resize(infile.tellg());
        infile.seekg(0, std::ios::beg);
        infile.read(&result[0], result.size());
        infile.close();
        return result;
    }

    std::unordered_map<Shader::Type, std::string> VulkanShader::Preprocess(const std::string& source) {
        std::unordered_map<Shader::Type, std::string> shaderSources;

        // Splits the shader source into separate shader stage sources

        const char* typeToken("#type");
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0); // start of shader type declaration line
        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos); // end of shader type declaration line
            CODI_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1; // start of shader type name (after "#type " keyword)
            std::string type = source.substr(begin, eol - begin);
            CODI_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specified!");

            size_t nextLinePos = source.find_first_of("\r\n", eol); // start of shader code after shader type declaration line
            CODI_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
            pos = source.find(typeToken, nextLinePos); // start of next shader type declaration line

            size_t shaderLength = (pos == std::string::npos) ? std::string::npos : pos - nextLinePos;
            shaderSources[Utils::ShaderTypeFromString(type)] = source.substr(nextLinePos, shaderLength);
        }

        return shaderSources;
    }

    void VulkanShader::CompileOrGetBinaries(const std::unordered_map<Shader::Type, std::string>& shaderSources) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        Utils::CreateCacheDirectoryIfNeeded();
        std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

        _Stages.clear();
        for (auto&& [type, source] : shaderSources) {
            std::filesystem::path shaderFilePath = _Filepath;
            std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(type));

            std::ifstream infile(cachedPath, std::ios::binary);
            if (infile.is_open()) {
                // Load cached SPIR-V binary
                infile.seekg(0, std::ios::end);
                std::streampos size = infile.tellg();
                infile.seekg(0, std::ios::beg);

                std::vector<uint32>& data = _Stages[type].SPIRV;
                data.resize(size / sizeof(uint32));
                infile.read((char*)data.data(), size);

                infile.close();
            }
            else {
                // Compile GLSL to SPIR-V
                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::ShaderTypeToShaderC(type), shaderFilePath.string().c_str(), options);
                if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                    CODI_CORE_ERROR(module.GetErrorMessage());
                    CODI_CORE_ASSERT(false);
                }

                _Stages[type].Source = source;
                _Stages[type].SPIRV = std::vector<uint32>(module.begin(), module.end());

                // Export to cached Binary
                std::ofstream outfile(cachedPath, std::ios::binary);
                if (outfile.is_open()) {
                    std::vector<uint32_t>& data = _Stages[type].SPIRV;
                    outfile.write((char*)data.data(), data.size() * sizeof(uint32));
                    outfile.flush();
                    outfile.close();
                }
            }
        }

        // Reflect on shader resources for each stage
        for (auto&& [type, stage] : _Stages)
            Reflect(type, stage.SPIRV);
    }

    void VulkanShader::Reflect(Shader::Type type, const std::vector<uint32>& shaderData) {
        if (shaderData.empty()) return;

        spirv_cross::Compiler compiler(shaderData);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        CODI_CORE_TRACE("VulkanShader::Reflect - {0} {1}", Utils::ShaderTypeToString(type), _Filepath.string());
        CODI_CORE_TRACE("   {0} uniform buffers", resources.uniform_buffers.size());
        CODI_CORE_TRACE("   {0} resources", resources.sampled_images.size());

        CODI_CORE_TRACE("Uniform buffers:");
        for (const spirv_cross::Resource& resource : resources.uniform_buffers) {
            const uint32 baseTypeID = resource.base_type_id;
            const spirv_cross::SPIRType& bufferType = compiler.get_type(baseTypeID);
            uint32 blockSize = (uint32)compiler.get_declared_struct_size(bufferType);
            uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            uint32 set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

            UniformBlock block;
            block.Name = resource.name;
            block.Binding = binding;
            block.Set = set;
            block.Size = blockSize;

            for (uint32 m = 0; m < bufferType.member_types.size(); m++) {
                UniformMember member;
                // member name
                member.Name = compiler.get_member_name(baseTypeID, m);
                // member offset (reflect offset decoration)
                // Use spirv_cross's get_member_decoration - offset decoration
                uint32 offset = compiler.get_member_decoration(baseTypeID, m, spv::DecorationOffset);
                member.Offset = offset;

                // member size: derive from member type
                const spirv_cross::SPIRType& memType = compiler.get_type(bufferType.member_types[m]);
                uint32 memSize = 0;
                if (memType.basetype == spirv_cross::SPIRType::Float) {
                    memSize = 4 * (memType.vecsize * memType.columns);
                }
                else if (memType.basetype == spirv_cross::SPIRType::Int) {
                    memSize = 4 * (memType.vecsize * memType.columns);
                }
                else {
                    // conservative fallback using struct size if it's a nested struct
                    memSize = (uint32)compiler.get_declared_struct_size(memType);
                }
                member.Size = memSize;

                CODI_CORE_TRACE("  UBO member: {0} offset={1} size={2}", member.Name, member.Offset, member.Size);
                block.Members.push_back(member);
            }

            _UniformBlocks.push_back(std::move(block));
        }

        // Sampled images
        for (const spirv_cross::Resource& resource : resources.sampled_images) {
            uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            uint32 set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            ImageBinding ib;
            ib.Name = resource.name;
            ib.Binding = binding;
            ib.Set = set;
            ib.Count = 1;
            _ImageBindings.push_back(ib);

            CODI_CORE_TRACE("Sampled image: {0} set={1} binding={2}", resource.name, set, binding);
        }
    }

    VkShaderModule VulkanShader::CreateModule(const std::vector<uint32>& shaderData) {
        if (shaderData.empty()) return VK_NULL_HANDLE;

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = shaderData.size() * sizeof(uint32);
        info.pCode = shaderData.data();

        VkShaderModule module;
        VkResult result = vkCreateShaderModule(logicalDevice, &info, VulkanRendererAPI::GetAllocator(), &module);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create shader module");

        return module;
    }

    void VulkanShader::CreateDescriptorSetLayouts() {
        if (_UniformBlocks.empty() && _ImageBindings.empty()) return;

        // Find maximum set index
        uint32 maxSet = 0;
        for (auto& b : _UniformBlocks) maxSet = std::max(maxSet, b.Set);
        for (auto& img : _ImageBindings) maxSet = std::max(maxSet, img.Set);

        _DescriptorSetLayouts.resize(uint64(maxSet + 1), VK_NULL_HANDLE);

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        for (uint32 set = 0; set <= maxSet; set++) {
            std::vector<VkDescriptorSetLayoutBinding> bindings;

            for (auto& b : _UniformBlocks) {
                if (b.Set != set) continue;
                VkDescriptorSetLayoutBinding l{};
                l.binding = b.Binding;
                l.descriptorCount = 1;
                l.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                l.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                bindings.push_back(l);
            }

            for (auto& img : _ImageBindings) {
                if (img.Set != set) continue;
                VkDescriptorSetLayoutBinding l{};
                l.binding = img.Binding;
                l.descriptorCount = img.Count;
                l.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                l.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                bindings.push_back(l);
            }

            if (bindings.empty()) continue;

            VkDescriptorSetLayoutCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            info.bindingCount = (uint32)bindings.size();
            info.pBindings = bindings.data();

            VkResult res = vkCreateDescriptorSetLayout(logicalDevice, &info, VulkanRendererAPI::GetAllocator(), &_DescriptorSetLayouts[set]);
            CODI_CORE_ASSERT(res == VK_SUCCESS, "Failed to create descriptor set layout");
        }
    }

    void VulkanShader::CreateDescriptorPoolAndSets() {
        if (_DescriptorSetLayouts.empty()) return;

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();
        uint32 framesInFlight = api.GetSwapchain()->GetMaxFramesInFlight();

        std::vector<VkDescriptorPoolSize> poolSizes;
        if (!_UniformBlocks.empty()) {
            // Count how many uniform buffers per set (we'll allocate frames_in_flight copies)
            uint32 ubCount = (uint32)_UniformBlocks.size();
            poolSizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ubCount * framesInFlight });
        }
        if (!_ImageBindings.empty()) {
            uint32 imgCount = (uint32)_ImageBindings.size();
            poolSizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imgCount * framesInFlight });
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = (uint32)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = (uint32)_DescriptorSetLayouts.size() * framesInFlight;

        VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, VulkanRendererAPI::GetAllocator(), &_DescriptorPool);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool");

        _DescriptorSets.resize(_DescriptorSetLayouts.size());
        for (uint32 s = 0; s < _DescriptorSetLayouts.size(); s++) {
            if (_DescriptorSetLayouts[s] == VK_NULL_HANDLE) continue;

            _DescriptorSets[s].resize(framesInFlight);
            std::vector<VkDescriptorSetLayout> layouts(framesInFlight, _DescriptorSetLayouts[s]);

            VkDescriptorSetAllocateInfo alloc{};
            alloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            alloc.descriptorPool = _DescriptorPool;
            alloc.descriptorSetCount = framesInFlight;
            alloc.pSetLayouts = layouts.data();

            result = vkAllocateDescriptorSets(logicalDevice, &alloc, _DescriptorSets[s].data());
            CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets");
        }
    }

    void VulkanShader::CreateUniformBuffers() {
        if (_UniformBlocks.empty()) return;

        for (auto& block : _UniformBlocks) {
            VulkanGlobalUniformRegistry::RegisteredBufferInfo regInfo;
            bool hasExternal = VulkanGlobalUniformRegistry::Get().Get(block.Set, block.Binding, regInfo);

            if (hasExternal)
                CreateExternalUniformBuffers(block);
            else
                CreateInternalUniformBuffers(block);
        }

        // Images remain to be bound by manually.
    }

    void VulkanShader::CreateExternalUniformBuffers(UniformBlock& block) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();
        uint32 framesInFlight = api.GetSwapchain()->GetMaxFramesInFlight();

        VulkanGlobalUniformRegistry::RegisteredBufferInfo regInfo;
        VulkanGlobalUniformRegistry::Get().Get(block.Set, block.Binding, regInfo);

        block.Buffer = regInfo.Buffer;
        block.Memory = regInfo.Memory;
        block.Mapped = regInfo.Mapped; // might be nullptr

        block.BufferInfos.resize(framesInFlight);
        for (uint32 frame = 0; frame < framesInFlight; frame++) {
            auto& info = block.BufferInfos[frame];
            info.buffer = block.Buffer;
            info.offset = 0; // external buffer is dedicated per resource (no offsets)
            info.range = block.Size;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = _DescriptorSets[block.Set][frame];
            write.dstBinding = block.Binding;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.pBufferInfo = &info;

            vkUpdateDescriptorSets(logicalDevice, 1, &write, 0, nullptr);
        }
    }

    void VulkanShader::CreateInternalUniformBuffers(UniformBlock& block) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();
        uint32 framesInFlight = api.GetSwapchain()->GetMaxFramesInFlight();

        VkDeviceSize totalSize = (VkDeviceSize)block.Size * framesInFlight;

        VkBufferCreateInfo bufInfo{};
        bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufInfo.size = totalSize;
        bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(logicalDevice, &bufInfo, VulkanRendererAPI::GetAllocator(), &block.Buffer);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create uniform buffer");

        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(logicalDevice, block.Buffer, &memReq);

        VkMemoryAllocateInfo alloc{};
        alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc.allocationSize = memReq.size;
        alloc.memoryTypeIndex = api.GetContext()->FindMemoryType(memReq.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        result = vkAllocateMemory(logicalDevice, &alloc, VulkanRendererAPI::GetAllocator(), &block.Memory);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate uniform buffer memory");

        result = vkBindBufferMemory(logicalDevice, block.Buffer, block.Memory, 0);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to bind uniform buffer memory");

        vkMapMemory(logicalDevice, block.Memory, 0, totalSize, 0, &block.Mapped);

        block.BufferInfos.resize(framesInFlight);

        for (uint32 frame = 0; frame < framesInFlight; frame++) {
            auto& info = block.BufferInfos[frame];
            info.buffer = block.Buffer;
            info.offset = (VkDeviceSize)block.Size * frame;
            info.range = block.Size;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = _DescriptorSets[block.Set][frame];
            write.dstBinding = block.Binding;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.pBufferInfo = &info;

            vkUpdateDescriptorSets(logicalDevice, 1, &write, 0, nullptr);
        }
    }

    void VulkanShader::DestroyUniformBuffers() {
        if (_UniformBlocks.empty()) return;

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        for (auto& block : _UniformBlocks) {
            if (block.Mapped) {
                vkUnmapMemory(logicalDevice, block.Memory);
                block.Mapped = nullptr;
            }
            if (block.Buffer != VK_NULL_HANDLE) {
                vkDestroyBuffer(logicalDevice, block.Buffer, VulkanRendererAPI::GetAllocator());
                block.Buffer = VK_NULL_HANDLE;
            }
            if (block.Memory != VK_NULL_HANDLE) {
                vkFreeMemory(logicalDevice, block.Memory, VulkanRendererAPI::GetAllocator());
                block.Memory = VK_NULL_HANDLE;
            }
        }
        _UniformBlocks.clear();
    }

    VulkanShader::UniformBlock* VulkanShader::FindBlockByMemberName(const std::string& name, uint32& outOffset, uint32& outSize) {
        // Accept either "member" (search through all UBO members) or "Block.member" (explicit)
        std::string blockPart, memberPart;
        uint64 dot = name.find('.');
        if (dot != std::string::npos) {
            blockPart = name.substr(0, dot);
            memberPart = name.substr(dot + 1);
        }
        else {
            blockPart = "";
            memberPart = name;
        }

        for (auto& block : _UniformBlocks) {
            if (!blockPart.empty() && block.Name != blockPart) continue;
            for (auto& m : block.Members) {
                if (m.Name == memberPart) {
                    outOffset = m.Offset;
                    outSize = m.Size;
                    return &block;
                }
            }
        }
        return nullptr;
    }

    template<typename T>
    void VulkanShader::UpdateUniform(const std::string& name, const T& value) {
        uint32 offset, size;
        UniformBlock* block = FindBlockByMemberName(name, offset, size);
        if (!block) {
            CODI_CORE_WARN("Uniform '{0}' not found", name);
            return;
        }

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        uint32 frame = api.GetCurrentFrameIndex();

        uint8* dst = (uint8*)block->Mapped + frame * block->Size + offset;
        memcpy(dst, &value, std::min<uint32>(size, (uint32)sizeof(T)));
    }

    void VulkanShader::SetInt(const std::string& name, const int32 value) { UpdateUniform(name, value); }

    void VulkanShader::SetIntArray(const std::string& name, int32* values, uint32 count) {
        uint32 offset, size;
        UniformBlock* block = FindBlockByMemberName(name, offset, size);
        if (!block) {
            CODI_CORE_WARN("Uniform '{0}' not found", name);
            return;
        }

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        uint32 frame = api.GetCurrentFrameIndex();

        uint8* dst = (uint8*)block->Mapped + (size_t)frame * block->Size + offset;
        uint32 bytes = count * sizeof(int32);
        memcpy(dst, values, std::min(bytes, size));
    }

    void VulkanShader::SetFloat(const std::string& name, const float32 value) { UpdateUniform(name, value); }

    void VulkanShader::SetFloat2(const std::string& name, const glm::vec2& value) { UpdateUniform(name, value); }

    void VulkanShader::SetFloat3(const std::string& name, const glm::vec3& value) { UpdateUniform(name, value); }

    void VulkanShader::SetFloat4(const std::string& name, const glm::vec4& value) { UpdateUniform(name, value); }

    void VulkanShader::SetMat4(const std::string& name, const glm::mat4& value) { UpdateUniform(name, value); }

} // namespace Codi
