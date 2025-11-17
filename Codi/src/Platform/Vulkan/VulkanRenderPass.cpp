#include "codipch.h"
#include "VulkanRenderPass.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    VulkanRenderPass::VulkanRenderPass(const VulkanRenderPassSpecification& spec) : _Specification(spec) {}

    void VulkanRenderPass::Create() {
        CODI_CORE_INFO("Creating Vulkan RenderPass...");

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());

        // Color attachement
        VkAttachmentDescription colorAttachement{};
        colorAttachement.format = _Specification.ColorFormat;   // TODO: configurable
        colorAttachement.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachement.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachement.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachement.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachement.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachement.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;     // Do not expect any particular layout before render pass starts
        colorAttachement.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Transitioned to after the render pass

        VkAttachmentReference colorAttachementReference{};
        colorAttachementReference.attachment = 0;
        colorAttachementReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Depth attachement
        VkAttachmentDescription depthAttachement{};
        depthAttachement.format = api.GetContext()->GetDepthFormat();
        depthAttachement.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachement.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachement.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachement.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachement.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachement.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachement.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachementReference{};
        depthAttachementReference.attachment = 1;
        depthAttachementReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // TODO: other attachment types (input, resolve, preserve)

        // Subpass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachementReference;
        subpass.pDepthStencilAttachment = &depthAttachementReference;

        // Input from a shader
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = nullptr;

        // Attachment used for multisampling color attachments
        subpass.pResolveAttachments = nullptr;
        subpass.pPreserveAttachments = nullptr;

        // RenderPass dependencies
        // TODO: make this configurable
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags = 0;

        // RenderPass create
        VkAttachmentDescription attachments[2] = { colorAttachement, depthAttachement };

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = attachments;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(api.GetContext()->GetLogicalDevice(), &createInfo, VulkanRendererAPI::GetAllocator(), &_Handle);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan RenderPass!");

        CODI_CORE_INFO("Vulkan RenderPass created successfully.");
    }

    void VulkanRenderPass::Destroy() {
        CODI_CORE_INFO("Destroying Vulkan render pass...");

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());

        if (_Handle)
            vkDestroyRenderPass(api.GetContext()->GetLogicalDevice(), _Handle, VulkanRendererAPI::GetAllocator());

        _Handle = VK_NULL_HANDLE;
    }

    void VulkanRenderPass::Begin(VulkanCommandBuffer* commandBuffer, VkFramebuffer frameBuffer) {
        CODI_CORE_ASSERT(commandBuffer->GetState() == VulkanCommandBuffer::State::Recording, "VulkanCommandBuffer is in an invalid state!");

        VkRenderPassBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = _Handle;
        info.framebuffer = frameBuffer;
        info.renderArea.offset.x = _Specification.X;
        info.renderArea.offset.y = _Specification.Y;
        info.renderArea.extent.width = _Specification.W;
        info.renderArea.extent.height = _Specification.H;

        VkClearValue clearValues[2];
        clearValues[0].color.float32[0] = _Specification.R;
        clearValues[0].color.float32[1] = _Specification.G;
        clearValues[0].color.float32[2] = _Specification.B;
        clearValues[0].color.float32[3] = _Specification.A;
        clearValues[1].depthStencil.depth = _Specification.Depth;
        clearValues[1].depthStencil.stencil = _Specification.Stencil;

        info.clearValueCount = 2;
        info.pClearValues = clearValues;

        vkCmdBeginRenderPass(commandBuffer->GetHandle(), &info, VK_SUBPASS_CONTENTS_INLINE);
        commandBuffer->SetState(VulkanCommandBuffer::State::InRenderPass);
    }

    void VulkanRenderPass::End(VulkanCommandBuffer* commandBuffer) {
        CODI_CORE_ASSERT(commandBuffer->GetState() == VulkanCommandBuffer::State::InRenderPass, "VulkanCommandBuffer is in an invalid state!");

        vkCmdEndRenderPass(commandBuffer->GetHandle());
        commandBuffer->SetState(VulkanCommandBuffer::State::Recording);
    }

    void VulkanRenderPass::SetSize(uint32 w, uint32 h) {
        _Specification.W = w;
        _Specification.H = h;
    }

} // namespace Codi
