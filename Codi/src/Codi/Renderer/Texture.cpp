#include "codipch.h"
#include "Texture.h"

#include "Codi/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanTexture.h"

namespace Codi {
    Shared<Texture2D> Texture2D::Create(uint32 width, uint32 height, void* data) {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Share<VulkanTexture2D>(width, height, data);
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }

    Shared<Texture2D> Texture2D::Create(const std::string& path) {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Share<VulkanTexture2D>(path);
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }
} // namespace Codi
