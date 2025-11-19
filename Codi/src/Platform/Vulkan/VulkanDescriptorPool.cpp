#include "codipch.h"
#include "VulkanDescriptorPool.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

#include <imgui.h>

namespace Codi {

    void VulkanDescriptorPool::Create() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        VkDescriptorPoolSize poolSizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000 * static_cast<uint32>(IM_ARRAYSIZE(poolSizes));
        poolInfo.poolSizeCount = static_cast<uint32>(IM_ARRAYSIZE(poolSizes));
        poolInfo.pPoolSizes = poolSizes;

        VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, VulkanRendererAPI::GetAllocator(), &_Handle);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan Descriptor Pool!");
    }

    void VulkanDescriptorPool::Destroy() {
        if (!_Handle) return;

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        vkDestroyDescriptorPool(logicalDevice, _Handle, VulkanRendererAPI::GetAllocator());
        _Handle = VK_NULL_HANDLE;
    }

} // namespace Codi
