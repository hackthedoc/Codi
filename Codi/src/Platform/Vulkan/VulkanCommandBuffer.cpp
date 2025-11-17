#include "codipch.h"
#include "VulkanCommandBuffer.h"

namespace Codi {

    void VulkanCommandBuffer::Allocate(VkDevice logicalDevice, VkCommandPool pool, bool isPrimary) {
        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = pool;
        info.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        info.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(logicalDevice, &info, &_Handle);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan command buffer!");

        _State = State::Ready;
    }

    void VulkanCommandBuffer::Free(VkDevice logicalDevice, VkCommandPool pool) {
        vkFreeCommandBuffers(logicalDevice, pool, 1, &_Handle);
        _Handle = VK_NULL_HANDLE;
        _State = State::NotAllocated;
    }

    void VulkanCommandBuffer::Begin(bool isSingleUse, bool isRenderPassContinue, bool isSimultaneousUse) {
        CODI_CORE_ASSERT(_State == State::Ready || _State == State::Submitted, "VulkanCommandBuffer::Begin() called in invalid state!");

        VkCommandBufferBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags = 0;
        if (isSingleUse) info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if (isRenderPassContinue) info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        if (isSimultaneousUse) info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        VkResult result = vkBeginCommandBuffer(_Handle, &info);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin Vulkan command buffer recording!");

        _State = State::Recording;
    }


    void VulkanCommandBuffer::End() {
        CODI_CORE_ASSERT(_State == State::Recording, "VulkanCommandBuffer::End() called in invalid state!");

        VkResult result = vkEndCommandBuffer(_Handle);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to end Vulkan command buffer recording!");

        _State = State::RecordingEnded;
    }

    void VulkanCommandBuffer::Submit(const SubmitInfo& submitInfo) {
        CODI_CORE_ASSERT(_State == State::RecordingEnded, "Command buffer must be ended before submission!");

        VkSubmitInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        info.commandBufferCount = 1;
        info.pCommandBuffers = &_Handle;

        info.waitSemaphoreCount = submitInfo.WaitSemaphores.size();
        info.pWaitSemaphores = submitInfo.WaitSemaphores.empty() ? nullptr : submitInfo.WaitSemaphores.data();
        info.pWaitDstStageMask = submitInfo.WaitStages.empty() ? nullptr : submitInfo.WaitStages.data();

        info.signalSemaphoreCount = submitInfo.SignalSemaphore.size();
        info.pSignalSemaphores = submitInfo.SignalSemaphore.empty() ? nullptr : submitInfo.SignalSemaphore.data();


        VkResult result = vkQueueSubmit(submitInfo.Queue, 1, &info, submitInfo.Fence);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit Vulkan command buffer!");

        if (submitInfo.WaitIdle)
            vkQueueWaitIdle(submitInfo.Queue);

        _State = State::Submitted;
    }

    void VulkanCommandBuffer::Reset(bool releaseResources) {
        CODI_CORE_ASSERT(_State != State::Recording, "Cannot reset command buffer while recording!");

        VkCommandBufferResetFlags flags = releaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0;
        VkResult result = vkResetCommandBuffer(_Handle, flags);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to reset Vulkan command buffer!");

        _State = State::Ready;
    }

} // namespace Codi
