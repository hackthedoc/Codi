#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Codi {

    enum class VulkanImageType {
        ColorAttachment,
        DepthAttachment,
        Texture,
        Storage,
    };

    struct VulkanImageSpecification {
        uint32 Width = 0;
        uint32 Height = 0;
        VkFormat Format = VK_FORMAT_UNDEFINED;
        VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags Usage = 0;
        VkMemoryPropertyFlags MemoryFlags = 0;
        bool CreateView = false;
        VkSampleCountFlagBits Samples = VK_SAMPLE_COUNT_1_BIT;
        VkImageAspectFlags AspectFlags = 0;
        VulkanImageType Type = VulkanImageType::ColorAttachment;
    };

    class VulkanImage {
    public:
        VulkanImage() = default;
        VulkanImage(const VulkanImageSpecification& spec);
        ~VulkanImage() = default;

        void Destroy();

        inline VkImage Get() const { return _Image; }
        inline VkImageView GetView() const { return _ImageView; }

    private:
        void CreateImage(const VulkanImageSpecification& spec);
        void CreateImageView(VkFormat format, VkImageAspectFlags apsectFlags);

        uint32 FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) const;

    private:
        VkImage _Image = VK_NULL_HANDLE;
        VkImageView _ImageView = VK_NULL_HANDLE;
        VkDeviceMemory _Memory = VK_NULL_HANDLE;

        VkExtent2D _Extent = { 0, 0 };
        VulkanImageType _Type = VulkanImageType::ColorAttachment;
    };

} // namespace Codi
