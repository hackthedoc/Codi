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
        struct StageData {
            std::string Source;
            std::vector<uint32> SPIRV;
            VkShaderModule Module = VK_NULL_HANDLE;
        };

        struct UniformMember {
            std::string Name;
            uint32 Offset = 0;
            uint32 Size = 0;
        };

        struct UniformBlock {
            std::string Name;
            uint32 Binding = 0;
            uint32 Set = 0;
            uint32 Size = 0;

            VkBuffer Buffer = VK_NULL_HANDLE;
            VkDeviceMemory Memory = VK_NULL_HANDLE;
            void* Mapped = nullptr;   // persistent mapped pointer (optional)

            std::vector<UniformMember> Members;
            std::vector<VkDescriptorBufferInfo> BufferInfos;
        };

        struct ImageBinding {
            std::string Name;
            uint32 Binding = 0;
            uint32 Set = 0;
            uint32 Count = 1;
        };

    private:
        // Shader utilities
        std::string ReadFile(const std::filesystem::path& filepath);
        std::unordered_map<Shader::Type, std::string> Preprocess(const std::string& source);
        void CompileOrGetBinaries(const std::unordered_map<Shader::Type, std::string>& shaderSources);
        void Reflect(Shader::Type type, const std::vector<uint32>& shaderData);

        VkShaderModule CreateModule(const std::vector<uint32>& shaderData);

        // Descriptor & buffer setup
        void CreateDescriptorSetLayouts();
        void CreateDescriptorPoolAndSets();
        void CreateUniformBuffers();
        void CreateExternalUniformBuffers(UniformBlock& block);
        void CreateInternalUniformBuffers(UniformBlock& block);
        void DestroyUniformBuffers();

        UniformBlock* FindBlockByMemberName(const std::string& name, uint32& outOffset, uint32& outSize);

        // Uniform update helper
        template<typename T>
        void UpdateUniform(const std::string& name, const T& value);

    private:
        std::filesystem::path _Filepath;
        std::string _Name;

        std::unordered_map<Type, StageData> _Stages;

        std::vector<UniformBlock> _UniformBlocks;
        std::vector<ImageBinding> _ImageBindings;

        VkDescriptorPool _DescriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayout> _DescriptorSetLayouts; // per set
        std::vector<std::vector<VkDescriptorSet>> _DescriptorSets; // per-set, per-frame

        Shared<VulkanPipeline> _Pipeline;
    };

} // namespace Codi
