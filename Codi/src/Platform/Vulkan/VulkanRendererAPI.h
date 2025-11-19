#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanGraphicsContext.h"
#include "Platform/Vulkan/VulkanSwapchain.h"
#include "Platform/Vulkan/VulkanRenderPass.h"
#include "Platform/Vulkan/VulkanFramebuffers.h"
#include "Platform/Vulkan/VulkanCommandPool.h"
#include "Platform/Vulkan/VulkanSync.h"

#include <vulkan/vulkan.h>

namespace Codi {

    class VulkanRendererAPI : public RendererAPI {
    public:
        virtual void Init() override;
        virtual void Shutdown() override;

        /// @return Wether the current frame is being skipped or not
        virtual bool BeginFrame(float32 deltatime) override;
        /// @return Wether the current frame is being skipped or not
        virtual bool EndFrame(float32 deltatime) override;

        virtual void OnWindowResize(uint32 width, uint32 height) override;
        virtual void SetViewport(float32 x, float32 y, float32 width, float32 height) override;

        inline VkViewport GetViewport() const { return _Viewport; }
        inline VkRect2D GetScissor() const { return _Scissor; }

        inline VulkanGraphicsContext* GetContext() { return _Context.get(); }
        inline VulkanSwapchain* GetSwapchain() { return _Swapchain.get(); }
        inline VulkanRenderPass* GetMainRenderPass() { return _MainRenderPass.get(); }
        inline VulkanFramebuffers* GetFramebuffers() { return _Framebuffers.get(); }

        static VkAllocationCallbacks* GetAllocator() { return nullptr; }

    private:
        void RecreateSwapchain();

    private:
        uint32 _CurrentFrameIndex = 0;
        bool _SwapchainNeedsRecreation = false;
        VkViewport _Viewport{};
        VkRect2D _Scissor{};

        Owned<VulkanGraphicsContext> _Context;
        Owned<VulkanSwapchain> _Swapchain = nullptr;
        Owned<VulkanRenderPass> _MainRenderPass = nullptr;
        Owned<VulkanFramebuffers> _Framebuffers = nullptr;
        Owned<VulkanCommandPool> _CommandPool = nullptr;
        std::vector<Owned<VulkanCommandBuffer>> _CommandBuffers;
        Owned<VulkanSync> _Sync = nullptr;
    };

} // namespace Codi
