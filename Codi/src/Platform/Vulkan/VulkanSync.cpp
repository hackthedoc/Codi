#include "codipch.h"
#include "VulkanSync.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    void VulkanSync::Create() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        uint32 maxFramesInFlight = api.GetSwapchain()->GetMaxFramesInFlight();
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        _SyncObjects.ImageAvailable.resize(maxFramesInFlight);
        _SyncObjects.RenderFinished.resize(maxFramesInFlight);
        _SyncObjects.InFlight.resize(maxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start signaled so the first frame doesn't wait

        for (uint32 i = 0; i < maxFramesInFlight; i++) {
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &_SyncObjects.ImageAvailable[i]);
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &_SyncObjects.RenderFinished[i]);
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &_SyncObjects.InFlight[i]);
        }
    }

    void VulkanSync::Destroy() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        uint32 maxFramesInFlight = api.GetSwapchain()->GetMaxFramesInFlight();
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        for (uint32 i = 0; i < maxFramesInFlight; i++) {
            if (_SyncObjects.ImageAvailable[i]) vkDestroySemaphore(logicalDevice, _SyncObjects.ImageAvailable[i], nullptr);
            if (_SyncObjects.RenderFinished[i]) vkDestroySemaphore(logicalDevice, _SyncObjects.RenderFinished[i], nullptr);
            if (_SyncObjects.InFlight[i]) vkDestroyFence(logicalDevice, _SyncObjects.InFlight[i], nullptr);
        }
    }

} // namespace Codi
