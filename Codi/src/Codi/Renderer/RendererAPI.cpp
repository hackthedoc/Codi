#include "codipch.h"
#include "RendererAPI.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    RendererAPI::API RendererAPI::_API = RendererAPI::API::Vulkan;

    Owned<RendererAPI> RendererAPI::Create() {
        switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return Own<VulkanRendererAPI>();
        }

        CODI_CORE_ASSERT(false, "Unknown RendererAPI::API!");
        return nullptr;
    }

} // namespace Codi
