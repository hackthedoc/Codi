#include "codipch.h"
#include "VulkanImage.h"

#include "Platform/Vulkan/VulkanGraphicsContext.h" 
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    VulkanImage::VulkanImage(VulkanGraphicsContext* context, const VulkanImageSpecification& spec)
        : _Context(context)
        , _Extent({ spec.Width, spec.Height })
        , _Type(spec.Type)
    {
        CreateImage(spec);
        if (spec.CreateView)
            CreateImageView(spec.Format, spec.AspectFlags);
    }

    void VulkanImage::CreateImage(const VulkanImageSpecification& spec) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = spec.Width;
        imageInfo.extent.height = spec.Height;
        imageInfo.extent.depth = 1;     // TODO: support configurable depth
        imageInfo.mipLevels = 4;        // TODO: support mip mapping
        imageInfo.arrayLayers = 1;      // TODO: support number of layers in the image
        imageInfo.format = spec.Format;
        imageInfo.tiling = spec.Tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = spec.Usage;
        imageInfo.samples = spec.Samples;                   // TODO: support configurable sample count
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // TODO: support configurable sharing mode

        VkResult result = vkCreateImage(_Context->GetLogicalDevice(), &imageInfo, VulkanRendererAPI::GetAllocator(), &_Image);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image!");

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(_Context->GetLogicalDevice(), _Image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, spec.MemoryFlags);

        result = vkAllocateMemory(_Context->GetLogicalDevice(), &allocInfo, VulkanRendererAPI::GetAllocator(), &_Memory);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate image memory!");

        vkBindImageMemory(_Context->GetLogicalDevice(), _Image, _Memory, 0);
    }

    void VulkanImage::Destroy() {
        if (_ImageView)
            vkDestroyImageView(_Context->GetLogicalDevice(), _ImageView, VulkanRendererAPI::GetAllocator());

        if (_Image)
            vkDestroyImage(_Context->GetLogicalDevice(), _Image, VulkanRendererAPI::GetAllocator());

        if (_Memory)
            vkFreeMemory(_Context->GetLogicalDevice(), _Memory, VulkanRendererAPI::GetAllocator());

        _ImageView = VK_NULL_HANDLE;
        _Image = VK_NULL_HANDLE;
        _Memory = VK_NULL_HANDLE;
    }

    void VulkanImage::CreateImageView(VkFormat format, VkImageAspectFlags apsectFlags) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  // TODO: make configurable
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = apsectFlags;

        // TODO: make configurable
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(_Context->GetLogicalDevice(), &viewInfo, VulkanRendererAPI::GetAllocator(), &_ImageView);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create image view!");
    }

    uint32 VulkanImage::FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) const {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(_Context->GetPhysicalDevice(), &memProperties);

        for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & BIT(i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        CODI_CORE_ASSERT(false, "Failed to find suitable memory type for Vulkan image!");
        return 0;
    }

} // namespace Codi
