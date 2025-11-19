#pragma once

#include "Codi/Core/Core.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"

#include <vulkan/vulkan.h>

namespace Codi {

    class VulkanPipeline {
    public:
        VulkanPipeline() = default;
        ~VulkanPipeline() = default;

        void Create(std::vector<VkVertexInputAttributeDescription> attributes, std::vector<VkDescriptorSetLayout> descriptorSetLayouts, std::vector<VkPipelineShaderStageCreateInfo> stages, uint32 stride, bool isWireframe);
        void Destroy();

        void Bind(VulkanCommandBuffer* commandBuffer, VkPipelineBindPoint bindPoint);

        inline VkPipelineLayout GetLayout() const { return _Layout; }
        inline VkPipeline GetHandle() const { return _Handle; }

    private:
        VkPipelineLayout _Layout = VK_NULL_HANDLE;
        VkPipeline _Handle = VK_NULL_HANDLE;
    };

} // namespace Codi
