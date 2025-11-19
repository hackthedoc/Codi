#include "codipch.h"
#include "Buffer.h"

#include "Codi/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace Codi {

    Shared<VertexBuffer> VertexBuffer::Create(uint32 size) {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Own<VulkanVertexBuffer>(size);
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }

    Shared<VertexBuffer> VertexBuffer::Create(void* vertices, uint32 size) {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Own<VulkanVertexBuffer>(vertices, size);
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }

    Shared<IndexBuffer> IndexBuffer::Create(uint32* indices, uint32 count) {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Own<VulkanIndexBuffer>(indices, count);
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }

    Shared<UniformBuffer> UniformBuffer::Create(uint32 size, uint32 binding) {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Own<VulkanUniformBuffer>(size, binding);
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }
} // namespace Codi
