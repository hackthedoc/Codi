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
        rpSpec.R = 0.6f;
        rpSpec.G = 0.0f;
        rpSpec.B = 0.0f;
        rpSpec.A = 1.0f;
        rpSpec.Depth = 1.0f;
        rpSpec.Stencil = 0;

        _MainRenderPass = Own<VulkanRenderPass>(rpSpec);
        _MainRenderPass->Create();

        // Frame buffers
        _Framebuffers = Own<VulkanFramebuffers>();
        _Framebuffers->Create();
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

} // namespace Codi
