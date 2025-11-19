#include "codipch.h"
#include "Shader.h"

#include "Codi/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanShader.h"

namespace Codi {

    Shared<Shader> Shader::Create(const std::filesystem::path& filepath) {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Own<VulkanShader>(filepath);
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }

    Shared<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Own<VulkanShader>(name, vertexSrc, fragmentSrc);
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }

} // namespace Codi
