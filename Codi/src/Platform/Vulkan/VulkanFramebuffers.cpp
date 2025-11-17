#include "codipch.h"
#include "VulkanFramebuffers.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    void VulkanFramebuffers::Create() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());

        const uint32 imageCount = api.GetSwapchain()->GetImageCount();
        _Framebuffers.resize(api.GetSwapchain()->GetImageCount());

        for (uint32 i = 0; i < imageCount; i++) {
            VkImageView attachments[] = {
                api.GetSwapchain()->GetFrameImageView(i),
                api.GetSwapchain()->GetDepthAttachment()->GetView()
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = api.GetMainRenderPass()->GetHandle();
            framebufferInfo.attachmentCount = 2;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = api.GetSwapchain()->GetExtent().width;
            framebufferInfo.height = api.GetSwapchain()->GetExtent().height;
            framebufferInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(api.GetContext()->GetLogicalDevice(), &framebufferInfo, VulkanRendererAPI::GetAllocator(), &_Framebuffers[i]);
            CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer!");
        }

        CODI_CORE_INFO("Created {0} Vulkan framebuffers.", _Framebuffers.size());
    }

    void VulkanFramebuffers::Destroy() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());

        for (auto fb : _Framebuffers)
            vkDestroyFramebuffer(api.GetContext()->GetLogicalDevice(), fb, VulkanRendererAPI::GetAllocator());
        _Framebuffers.clear();
    }

} // namespace Codi
