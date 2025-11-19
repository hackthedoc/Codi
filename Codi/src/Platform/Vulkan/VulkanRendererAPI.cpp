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
        rpSpec.R = 0.0863f;
        rpSpec.G = 0.0902f;
        rpSpec.B = 0.1137f;
        rpSpec.A = 1.0f;
        rpSpec.Depth = 1.0f;
        rpSpec.Stencil = 0;

        _MainRenderPass = Own<VulkanRenderPass>(rpSpec);
        _MainRenderPass->Create();

        // Frame buffers
        _Framebuffers = Own<VulkanFramebuffers>();
        _Framebuffers->Create();

        _CurrentFrameIndex = 0;
        _SwapchainNeedsRecreation = false;
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

    bool VulkanRendererAPI::BeginFrame() {
        CODI_CORE_TRACE("BeginFrame - _CurrentFrameIndex = {0}", _CurrentFrameIndex);

        if (_SwapchainNeedsRecreation) {
            RecreateSwapchain();
            _SwapchainNeedsRecreation = false;
            return true; // skip this frame
        }

        // Get IN FLIGHT objects
        VkFence inFlightFence = _Sync->GetInFlight(_CurrentFrameIndex);
        VkSemaphore inFlightImageAvailableSemaphore = _Sync->GetImageAvailable(_CurrentFrameIndex);

        // Acquire next image from swapchain
        VulkanSwapResult result = _Swapchain->AcquireNextImage(inFlightImageAvailableSemaphore, VK_NULL_HANDLE, UINT64_MAX);

        if (result == VulkanSwapResult::OutOfDate || result == VulkanSwapResult::SubOptimal) {
            _SwapchainNeedsRecreation = true;
            return true; // skip this frame
        }

        // Wait for previous frame to finish
        vkWaitForFences(_Context->GetLogicalDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(_Context->GetLogicalDevice(), 1, &inFlightFence);

        // Begin command buffer
        _CommandBuffers[_CurrentFrameIndex]->Reset();
        _CommandBuffers[_CurrentFrameIndex]->Begin();

        // Begin render pass
        _MainRenderPass->Begin(_CommandBuffers[_CurrentFrameIndex].get(), _Framebuffers->Get(_Swapchain->GetCurrentImageIndex()));

        SetViewport(0, 0, (float32)_Swapchain->GetWidth(), (float32)_Swapchain->GetHeight());

        return false;
    }

    bool VulkanRendererAPI::EndFrame() {
        CODI_CORE_TRACE("EndFrame - _CurrentFrameIndex = {0}", _CurrentFrameIndex);

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

        return false;
    }

    void VulkanRendererAPI::OnWindowResize(uint32 width, uint32 height) {
        if (width == 0 || height == 0) return; // window minimized, ignore

        if (_Swapchain->GetWidth() == width && _Swapchain->GetHeight() == height)
            return; // size not changed, ignore

        _SwapchainNeedsRecreation = true;
    }

    void VulkanRendererAPI::SetViewport(float32 x, float32 y, float32 width, float32 height) {
        _Viewport.x = x;
        _Viewport.y = height - y; // inverted Y
        _Viewport.width = width;
        _Viewport.height = -height; // negative flips Y
        _Viewport.minDepth = 0.0f;
        _Viewport.maxDepth = 1.0f;

        _Scissor.offset = { (int32)x, (int32)y };
        _Scissor.extent = { (uint32)width, (uint32)height };

        vkCmdSetViewport(_CommandBuffers[_CurrentFrameIndex]->GetHandle(), 0, 1, &_Viewport);
        vkCmdSetScissor(_CommandBuffers[_CurrentFrameIndex]->GetHandle(), 0, 1, &_Scissor);
    }

    void VulkanRendererAPI::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32 indexCount) {
        VkCommandBuffer cmdBuffer = _CommandBuffers[_CurrentFrameIndex]->GetHandle();

        std::vector<VkBuffer> vertexBuffers;
        for (auto buffer : vertexArray->GetVertexBuffers())
            vertexBuffers.push_back(*(VkBuffer*)buffer->GetHandle());

        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers.data(), offsets);
        vkCmdBindIndexBuffer(cmdBuffer, *(VkBuffer*)vertexArray->GetIndexBuffer()->GetHandle(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0);
    }

    void VulkanRendererAPI::RecreateSwapchain() {
        Window* window = Application::Get().GetWindow();
        uint32 width = window->GetWidth();
        uint32 height = window->GetHeight();

        VulkanSwapchain::SetSupport(_Context->GetPhysicalDevice(), _Context->GetSurface());

        _Context->WaitDeviceIdle();

        _Framebuffers->Destroy();

        _Swapchain->Recreate(width, height);

        _MainRenderPass->SetSize(width, height);

        _Framebuffers->Create();

        for (auto& cmd : _CommandBuffers)
            cmd->Reset();   // reset all command buffers

        _CurrentFrameIndex = (_CurrentFrameIndex + 1) % _Swapchain->GetMaxFramesInFlight();

        CODI_CORE_WARN("Swapchain and framebuffers recreated for size {0}x{1}", width, height);
    }

} // namespace Codi
