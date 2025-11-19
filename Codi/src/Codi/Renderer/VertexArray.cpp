#include "codipch.h"
#include "VertexArray.h"

#include "Codi/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Codi {

    Shared<VertexArray> VertexArray::Create() {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Own<VulkanVertexArray>();
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }

} // namespace Codi
