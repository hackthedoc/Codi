#include "codipch.h"
#include "VulkanRendererAPI.h"

#include "Codi/Core/Application.h"

namespace Codi {

    void VulkanRendererAPI::Init() {
        Window* window = Application::Get().GetWindow();

        // Graphics Context
        VulkanDeviceRequirements deviceReq{};
        _Context = Own<VulkanGraphicsContext>(window->GetNativeWindow());
        _Context->Create(deviceReq);

        // Swapchain
        _Swapchain = Own<VulkanSwapchain>(_Context.get());
        _Swapchain->Create(window->GetWidth(), window->GetHeight());
        CODI_CORE_TRACE("Creating Swapchain of size {0}x{1}", window->GetWidth(), window->GetHeight());

        // Command Pool
        VulkanCommandPoolSpecification poolSpec{};
        poolSpec.QueueFamilyIndex = _Context->GetQueueIndices().GraphicsFamily;
        poolSpec.ResetCommandBuffer = true;
        poolSpec.Transient = false;
        _CommandPool = Own<VulkanCommandPool>(poolSpec);
        _CommandPool->Create();

        // Command buffers
        _CommandBuffers.resize(_Swapchain->GetMaxFramesInFlight());
        for (uint32 i = 0; i < _CommandBuffers.size(); i++) {
            _CommandBuffers[i] = Own<VulkanCommandBuffer>();
            _CommandBuffers[i]->Allocate(_Context->GetLogicalDevice(), _CommandPool->GetHandle(), true);
        }

        // Sync objects
        _Sync = Own<VulkanSync>();
        _Sync->Create();

        // Render pass
        VulkanRenderPassSpecification rpSpec{};
        rpSpec.X = 0;
        rpSpec.Y = 0;
        rpSpec.W = window->GetWidth();
        rpSpec.H = window->GetHeight();
        rpSpec.R = 0.741f;
        rpSpec.G = 0.576f;
        rpSpec.B = 0.976f;
        rpSpec.A = 1.0f;
        rpSpec.Depth = 1.0f;
        rpSpec.Stencil = 0;

        _MainRenderPass = Own<VulkanRenderPass>(rpSpec);
        _MainRenderPass->Create();

        // Frame buffers
        _Framebuffers = Own<VulkanFramebuffers>();
        _Framebuffers->Create();

        _CurrentFrameIndex = 0;
    }

    void VulkanRendererAPI::Shutdown() {
        _Context->WaitDeviceIdle();

        // Frame buffers
        _Framebuffers->Destroy();
        _Framebuffers = nullptr;

        // Render pass
        _MainRenderPass->Destroy();
        _MainRenderPass = nullptr;

        // Sync objects
        _Sync->Destroy();
        _Sync = nullptr;

        // Command buffers
        for (auto& cmd : _CommandBuffers)
            cmd->Free(_Context->GetLogicalDevice(), _CommandPool->GetHandle());
        _CommandBuffers.clear();

        // Command Pool
        _CommandPool->Destroy();
        _CommandPool = nullptr;

        // Swapchain
        _Swapchain->Destroy();
        _Swapchain = nullptr;

        // Graphics Context
        _Context->Destroy();
        _Context = nullptr;
    }

    void VulkanRendererAPI::BeginFrame(float32 deltatime) {
        // Get IN FLIGHT objects
        VkFence inFlightFence = _Sync->GetInFlight(_CurrentFrameIndex);
        VkSemaphore inFlightImageAvailableSemaphore = _Sync->GetImageAvailable(_CurrentFrameIndex);

        // Wait for previous frame to finish
        vkWaitForFences(_Context->GetLogicalDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(_Context->GetLogicalDevice(), 1, &inFlightFence);

        // Acquire next image from swapchain
        VulkanSwapResult result = _Swapchain->AcquireNextImage(inFlightImageAvailableSemaphore, VK_NULL_HANDLE, UINT64_MAX);

        if (result == VulkanSwapResult::OutOfDate) {
            RecreateSwapchain();
            return;
        }

        // Begin command buffer
        _CommandBuffers[_CurrentFrameIndex]->Reset();
        _CommandBuffers[_CurrentFrameIndex]->Begin();

        // Begin render pass
        _MainRenderPass->Begin(_CommandBuffers[_CurrentFrameIndex].get(), _Framebuffers->Get(_Swapchain->GetCurrentImageIndex()));
    }

    void VulkanRendererAPI::EndFrame(float32 deltatime) {
        // End render pass
        _MainRenderPass->End(_CommandBuffers[_CurrentFrameIndex].get());

        // End command buffer recording
        _CommandBuffers[_CurrentFrameIndex]->End();

        // Submit command buffer
        VulkanCommandBuffer::SubmitInfo submitInfo{};
        submitInfo.Queue = _Context->GetGraphicsQueue();
        submitInfo.Fence = _Sync->GetInFlight(_CurrentFrameIndex);
        submitInfo.WaitSemaphores.push_back(_Sync->GetImageAvailable(_CurrentFrameIndex));
        submitInfo.WaitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        submitInfo.SignalSemaphore.push_back(_Sync->GetRenderFinished(_CurrentFrameIndex));

        _CommandBuffers[_CurrentFrameIndex]->Submit(submitInfo);

        // Present image
        _Swapchain->Present(_Sync->GetRenderFinished(_CurrentFrameIndex));

        // Advance frame
        _CurrentFrameIndex = (_CurrentFrameIndex + 1) % _Swapchain->GetMaxFramesInFlight();
    }

    void VulkanRendererAPI::RecreateSwapchain() {
        Window* window = Application::Get().GetWindow();
        uint32 width = window->GetWidth();
        uint32 height = window->GetHeight();

        // Wait for the device to be idle before recreating
        _Context->WaitDeviceIdle();

        // Destroy old framebuffers
        _Framebuffers->Destroy();

        // Destroy swapchain (this also destroys depth attachment)
        _Swapchain->Destroy();

        // Recreate swapchain
        _Swapchain->Create(width, height);

        // Recreate framebuffers for the new swapchain images
        _Framebuffers->Create();

        // Update render pass size
        _MainRenderPass->SetSize(width, height);
    }

} // namespace Codi
