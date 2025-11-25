#include "codipch.h"
#include "VulkanDescriptorSet.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    void VulkanDescriptorSet::Create(const DescriptorSetCreateInfo& info) {
        CreateLayout(info);
        CreatePool(info);
        AllocateSets(info);
    }

    void VulkanDescriptorSet::Destroy() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        if (_Pool)
            vkDestroyDescriptorPool(logicalDevice, _Pool, VulkanRendererAPI::GetAllocator());

        if (_Layout)
            vkDestroyDescriptorSetLayout(logicalDevice, _Layout, VulkanRendererAPI::GetAllocator());

        _Pool = VK_NULL_HANDLE;
        _Layout = VK_NULL_HANDLE;
        _DescriptorSets.clear();
    }

    void VulkanDescriptorSet::UpdateImage(uint32 setIndex, uint32 binding, VkDescriptorImageInfo& imageInfo) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = _DescriptorSets[setIndex];
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(logicalDevice, 1, &write, 0, nullptr);
    }

    void VulkanDescriptorSet::UpdateBuffer(uint32 setIndex, uint32 binding, VkDescriptorBufferInfo& bufferInfo) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = _DescriptorSets[setIndex];
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(logicalDevice, 1, &write, 0, nullptr);
    }

    void VulkanDescriptorSet::CreateLayout(const DescriptorSetCreateInfo& info) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve(info.Bindings.size());

        for (auto& b : info.Bindings) {
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = b.Binding;
            binding.descriptorType = b.Type;
            binding.descriptorCount = b.DescriptorCount;
            binding.stageFlags = b.StageFlags;
            binding.pImmutableSamplers = nullptr;

            layoutBindings.push_back(binding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (uint32)layoutBindings.size();
        layoutInfo.pBindings = layoutBindings.data();

        VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, VulkanRendererAPI::GetAllocator(), &_Layout);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout!"); 
    }

    void VulkanDescriptorSet::CreatePool(const DescriptorSetCreateInfo& info) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.reserve(info.Bindings.size());

        for (auto& b : info.Bindings) {
            VkDescriptorPoolSize size{};
            size.type = b.Type;
            size.descriptorCount = b.DescriptorCount * info.SetCount;
            poolSizes.push_back(size);
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.maxSets = info.SetCount;
        poolInfo.poolSizeCount = (uint32)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();

        VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, VulkanRendererAPI::GetAllocator(), &_Pool);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool!");
    }

    void VulkanDescriptorSet::AllocateSets(const DescriptorSetCreateInfo& info) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        std::vector<VkDescriptorSetLayout> layouts(info.SetCount, _Layout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _Pool;
        allocInfo.descriptorSetCount = info.SetCount;
        allocInfo.pSetLayouts = layouts.data();

        _DescriptorSets.resize(info.SetCount);

        VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, _DescriptorSets.data());
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets!");
    }

} // namespace Codi
