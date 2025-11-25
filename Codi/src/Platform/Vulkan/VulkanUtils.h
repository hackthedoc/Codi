#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Codi::Utils {

    void CreateVulkanBuffer(uint32 size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* memory);
    void* UploadVulkanBuffer(const void* data, uint32 size, uint32 offset, VkDeviceMemory memory);
    void DestroyVulkanBuffer(VkBuffer* buffer, VkDeviceMemory* memory);

    void CreateVulkanImage(uint32 width, uint32 height, VkFormat format, VkImageUsageFlags usage, VkImage* outImage, VkDeviceMemory* outMemory);

    void UploadToVulkanImage(VkImage image, uint32 width, uint32 height, const void* pixels);

    VkImageView CreateVulkanImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask);

    uint32 FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);

    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    void CopyBufferToVulkanImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height);

} // namespace Codi::Utils
