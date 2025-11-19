#include "codipch.h"
#include "VulkanShader.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

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
            if (stage.Data.empty()) continue;
            stage.Module = CreateModule(stage.Data);
        }

        // Extract name from filepath
        _Name = filepath.stem().string();
    }

    VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : _Name(name), _Filepath(name) {
        std::unordered_map<Shader::Type, std::string> shaderSources = {
            { Shader::Type::Vertex, vertexSrc, },
            { Shader::Type::Fragment, fragmentSrc, }
        };

        CompileOrGetBinaries(shaderSources);

        for (auto& [type, stage] : _Stages) {
            if (stage.Data.empty()) continue;
            stage.Module = CreateModule(stage.Data);
        }
    }

    VulkanShader::~VulkanShader() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        _Pipeline->Destroy();
        _Pipeline = nullptr;

        for (auto& [stage, data] : _Stages)
            vkDestroyShaderModule(logicalDevice, data.Module, VulkanRendererAPI::GetAllocator());
    }

    void VulkanShader::Bind() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VulkanGraphicsContext* context = api.GetContext();
        uint32 imageIndex = api.GetCurrentFrameIndex();
        _Pipeline->Bind(api.GetCommandBuffer(imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS);
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

        // TODO: descriptor set layouts
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};

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
        _Pipeline->Create(attributeDescriptions, descriptorSetLayouts, stageCreateInfos, layout.GetStride(), false);
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

                std::vector<uint32>& data = _Stages[type].Data;
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
                _Stages[type].Data = std::vector<uint32>(module.begin(), module.end());

                // Export to cached Binary
                std::ofstream outfile(cachedPath, std::ios::binary);
                if (outfile.is_open()) {
                    std::vector<uint32_t>& data = _Stages[type].Data;
                    outfile.write((char*)data.data(), data.size() * sizeof(uint32));
                    outfile.flush();
                    outfile.close();
                }
            }
        }

        // Reflect on shader resources for each stage
        for (auto&& [type, stage] : _Stages)
            Reflect(type, stage.Data);
    }

    // Reflects on the SPIR-V binary to log uniform buffer and resource info
    void VulkanShader::Reflect(Shader::Type type, const std::vector<uint32>& shaderData) {
        if (shaderData.empty()) return;

        spirv_cross::Compiler compiler(shaderData);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        CODI_CORE_TRACE("VulkanShader::Reflect - {0} {1}", Utils::ShaderTypeToString(type), _Filepath.string());
        CODI_CORE_TRACE("   {0} uniform buffers", resources.uniform_buffers.size());
        CODI_CORE_TRACE("   {0} resources", resources.sampled_images.size());

        CODI_CORE_TRACE("Uniform buffers:");
        for (const spirv_cross::Resource& resource : resources.uniform_buffers) {
            const spirv_cross::SPIRType& bufferType = compiler.get_type(resource.base_type_id);
            uint32 bufferSize = compiler.get_declared_struct_size(bufferType);
            uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            int32 memberCount = bufferType.member_types.size();

            CODI_CORE_TRACE("  {0}", resource.name);
            CODI_CORE_TRACE("    Size = {0}", bufferSize);
            CODI_CORE_TRACE("    Binding = {0}", binding);
            CODI_CORE_TRACE("    Members = {0}", memberCount);
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

    void VulkanShader::SetInt(const std::string& name, const int32 value) {  }

    void VulkanShader::SetIntArray(const std::string& name, int32* values, uint32 count) {  }

    void VulkanShader::SetFloat(const std::string& name, const float32 value) {  }

    void VulkanShader::SetFloat2(const std::string& name, const glm::vec2& value) {  }

    void VulkanShader::SetFloat3(const std::string& name, const glm::vec3& value) {  }

    void VulkanShader::SetFloat4(const std::string& name, const glm::vec4& value) {  }

    void VulkanShader::SetMat4(const std::string& name, const glm::mat4& value) {  }


} // namespace Codi
