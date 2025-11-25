#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Codi {

    struct DescriptorSetLayoutBindingInfo {
        uint32 Binding;
        VkDescriptorType Type;
        VkShaderStageFlags StageFlags;
        uint32 DescriptorCount = 1;
    };

    struct DescriptorSetCreateInfo {
        std::vector<DescriptorSetLayoutBindingInfo> Bindings;
        uint32 SetCount = 1;
    };

    class VulkanDescriptorSet {
    public:
        VulkanDescriptorSet() = default;
        ~VulkanDescriptorSet() = default;

        void Create(const DescriptorSetCreateInfo& info);
        void Destroy();

        void UpdateImage(uint32 setIndex, uint32 binding, VkDescriptorImageInfo& imageInfo);
        void UpdateBuffer(uint32 setIndex, uint32 binding, VkDescriptorBufferInfo& bufferInfo);

        inline VkDescriptorPool GetPool() const { return _Pool; }
        inline VkDescriptorSetLayout GetLayout() const { return _Layout; }
        inline VkDescriptorSet Get(uint32 index) const { return _DescriptorSets[index]; }

    private:
        void CreateLayout(const DescriptorSetCreateInfo& info);
        void CreatePool(const DescriptorSetCreateInfo& info);
        void AllocateSets(const DescriptorSetCreateInfo& info);

    private:
        VkDescriptorPool _Pool = VK_NULL_HANDLE;
        VkDescriptorSetLayout _Layout = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> _DescriptorSets;
    };

} // namespace Codi
