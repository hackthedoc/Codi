#pragma once

#include "Codi/Core/Core.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"

#include <vulkan/vulkan.h>

namespace Codi {

    struct VulkanRenderPassSpecification {
        int32 X = 0, Y = 0;
        uint32 W = 0, H = 0;
        float32 R = 0.1f, G = 0.1f, B = 0.1f, A = 1.0f;
        float32 Depth = 1.0f;
        uint32 Stencil = 0;
        VkFormat ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
    };

    class VulkanRenderPass {
    public:
        VulkanRenderPass(const VulkanRenderPassSpecification& spec);
        ~VulkanRenderPass() = default;

        void Create();
        void Destroy();

        void Begin(VulkanCommandBuffer* commandBuffer, VkFramebuffer frameBuffer);
        void End(VulkanCommandBuffer* commandBuffer);

        void SetSize(uint32 w, uint32 h);

        inline VkRenderPass GetHandle() const { return _Handle; }

    private:
        VulkanRenderPassSpecification _Specification{};

        VkRenderPass _Handle = VK_NULL_HANDLE;
    };

} // namespace Codi
