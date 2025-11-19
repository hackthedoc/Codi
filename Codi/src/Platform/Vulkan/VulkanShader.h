#pragma once

#include "Codi/Renderer/Shader.h"

#include "Platform/Vulkan/VulkanPipeline.h"

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

namespace Codi {

    // TODO: Descriptors
    class VulkanShader : public Shader {
    public:
        struct StageData {
            std::string Source;
            std::vector<uint32> Data;
            VkShaderModule Module = VK_NULL_HANDLE;
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

    private:
        std::filesystem::path _Filepath;
        std::string _Name;

        std::unordered_map<Type, StageData> _Stages;

        Shared<VulkanPipeline> _Pipeline;
    };

} // namespace Codi
