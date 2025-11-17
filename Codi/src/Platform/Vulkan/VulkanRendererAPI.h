#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanGraphicsContext.h"
#include "Platform/Vulkan/VulkanSwapchain.h"

#include <vulkan/vulkan.h>

namespace Codi {

    class VulkanRendererAPI : public RendererAPI {
    public:
        virtual void Init() override;
        virtual void Shutdown() override;

        static Shared<VulkanGraphicsContext> GetContext() { return _Context; }

        static VkAllocationCallbacks* GetAllocator() { return nullptr; }

    private:
        static Shared<VulkanGraphicsContext> _Context;

        Owned<VulkanSwapchain> _Swapchain = nullptr;
    };

} // namespace Codi
