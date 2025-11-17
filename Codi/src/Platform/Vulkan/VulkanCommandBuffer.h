#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Codi {

    class VulkanCommandBuffer {
    public:
        enum class State {
            NotAllocated = 0,
            Ready,
            Recording,
            InRenderPass,
            RecordingEnded,
            Submitted
        };

        struct SubmitInfo {
            VkQueue Queue = VK_NULL_HANDLE;
            VkFence Fence = VK_NULL_HANDLE; // Optional
            std::vector<VkSemaphore> WaitSemaphores{};
            std::vector<VkPipelineStageFlags> WaitStages{};
            std::vector<VkSemaphore> SignalSemaphore{};
            bool WaitIdle = true;
        };

    public:
        VulkanCommandBuffer() = default;
        ~VulkanCommandBuffer() = default;

        void Allocate(VkDevice logicalDevice, VkCommandPool pool, bool isPrimary = true);
        void Free(VkDevice logicalDevice, VkCommandPool pool);

        void Begin(bool isSingleUse = false, bool isRenderPassContinue = false, bool isSimultaneousUse = false);
        void End();

        void Submit(const SubmitInfo& info);

        void Reset(bool releaseResources = false);

        inline void SetState(State state) { _State = state; }

        inline VkCommandBuffer GetHandle() const { return _Handle; }
        inline State GetState() const { return _State; }

    private:
        VkCommandBuffer _Handle = VK_NULL_HANDLE;
        State _State = State::NotAllocated;
    };

} // namespace Codi
