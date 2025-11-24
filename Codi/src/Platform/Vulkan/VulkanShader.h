#pragma once

#include "Codi/Renderer/Shader.h"

#include "Platform/Vulkan/VulkanPipeline.h"

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

namespace Codi {
    class VulkanUniformBuffer;

    // TODO: Descriptors
    class VulkanShader : public Shader {
    public:
        struct StageData {
            std::string Source;
            std::vector<uint32> Data;
            VkShaderModule Module = VK_NULL_HANDLE;
        };

        struct UniformMember {
            std::string Name;      // member name
            uint32 Offset;         // byte offset inside block
            uint32 Size;           // member size in bytes
        };

        struct UniformBlock {
            std::string Name;         // block name
            uint32 Binding;           // descriptor binding
            uint32 Set;               // descriptor set (usually 0)
            uint32 Size;              // total block size in bytes
            VkBuffer Buffer = VK_NULL_HANDLE;
            VkDeviceMemory Memory = VK_NULL_HANDLE;
            void* Mapped = nullptr;   // persistent mapped pointer
            std::vector<UniformMember> Members;
            std::vector<VkDescriptorBufferInfo> BufferInfos;
        };

        struct ImageBinding {
            std::string Name;
            uint32 Binding;
            uint32 Set;
            uint32 Count;
        };

    public:
        VulkanShader(const std::filesystem::path& filepath);
        VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~VulkanShader();

        virtual void Bind() override;

        virtual void CreatePipeline(Shared<VertexBuffer> quadVertexBuffer) override;

        virtual void SetInt(const std::string& name, const int32 value) override;
        virtual void SetIntArray(const std::string& name, int32* values, uint32 count) override;
        virtual void SetFloat(const std::string& name, const float32 value) override;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

        virtual const std::string& GetName() const override { return _Name; }

    private:
        std::string ReadFile(const std::filesystem::path& filepath);
        std::unordered_map<Shader::Type, std::string> Preprocess(const std::string& source);

        void CompileOrGetBinaries(const std::unordered_map<Shader::Type, std::string>& shaderSources);
        void Reflect(Shader::Type type, const std::vector<uint32>& shaderData);

        VkShaderModule CreateModule(const std::vector<uint32>& shaderData);

        void CreateDescriptorSetLayouts();
        void CreateDescriptorPoolAndSets();
        void CreateUniformBuffers();
        void DestroyUniformBuffers();

        UniformBlock* FindBlockByMemberName(const std::string& name, uint32& outOffset, uint32& outSize);

    private:
        std::filesystem::path _Filepath;
        std::string _Name;

        std::unordered_map<Type, StageData> _Stages;

        std::vector<UniformBlock> _UniformBlocks; // list of UBOs discovered
        std::vector<ImageBinding> _ImageBindings;

        VkDescriptorPool _DescriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayout> _DescriptorSetLayouts; // per set
        std::vector<std::vector<VkDescriptorSet>> _DescriptorSets; // per-set, per-frame

        Shared<VulkanPipeline> _Pipeline;
    };

} // namespace Codi
