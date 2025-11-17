#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Codi {

    struct VulkanSyncObjects {
        std::vector<VkSemaphore> ImageAvailable;
        std::vector<VkSemaphore> RenderFinished;
        std::vector<VkFence> InFlight;
    };

    class VulkanSync {
    public:
        VulkanSync() = default;
        ~VulkanSync() = default;

        void Create();
        void Destroy();

        inline VkSemaphore GetImageAvailable(uint32 index) const { return _SyncObjects.ImageAvailable[index]; }
        inline VkSemaphore GetRenderFinished(uint32 index) const { return _SyncObjects.RenderFinished[index]; }
        inline VkFence GetInFlight(uint32 index) const { return _SyncObjects.InFlight[index]; }

    private:
        VulkanSyncObjects _SyncObjects;
    };

} // namespace Codi
