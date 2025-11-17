#include "codipch.h"
#include "VulkanCommandPool.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    VulkanCommandPool::VulkanCommandPool(const VulkanCommandPoolSpecification& spec) : _Specification(spec) {}

    void VulkanCommandPool::Create() {
        CODI_CORE_INFO("Creating Vulkan command pool...");

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = _Specification.QueueFamilyIndex;

        poolInfo.flags = 0;
        if (_Specification.ResetCommandBuffer)
            poolInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        if (_Specification.Transient)
            poolInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VkResult result = vkCreateCommandPool(api.GetContext()->GetLogicalDevice(), &poolInfo, VulkanRendererAPI::GetAllocator(), &_Handle);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan command pool!");
    }

    void VulkanCommandPool::Destroy() {
        CODI_CORE_INFO("Destroying Vulkan command pool...");

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());

        if (_Handle)
            vkDestroyCommandPool(api.GetContext()->GetLogicalDevice(), _Handle, VulkanRendererAPI::GetAllocator());

        _Handle = VK_NULL_HANDLE;
    }

    void VulkanCommandPool::ResetPool() const {
        CODI_CORE_ASSERT(_Handle != VK_NULL_HANDLE, "Command pool not created!");

        CODI_CORE_INFO("Reseting Vulkan command pool...");

        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());

        VkResult result = vkResetCommandPool(api.GetContext()->GetLogicalDevice(), _Handle, 0);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to reset Vulkan command pool!");
    }

} // namespace Codi
