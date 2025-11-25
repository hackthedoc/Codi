#include "codipch.h"
#include "VulkanUtils.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi::Utils {

    void CreateVulkanBuffer(uint32 size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* memory) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VulkanGraphicsContext* context = api.GetContext();
        VkDevice logicalDevice = context->GetLogicalDevice();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateBuffer(logicalDevice, &bufferInfo, VulkanRendererAPI::GetAllocator(), buffer);

        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memReq);

        VkMemoryAllocateInfo memInfo{};
        memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memInfo.allocationSize = memReq.size;
        memInfo.memoryTypeIndex = context->FindMemoryType(
            memReq.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        vkAllocateMemory(logicalDevice, &memInfo, VulkanRendererAPI::GetAllocator(), memory);
        vkBindBufferMemory(logicalDevice, *buffer, *memory, 0);
    }

    void* UploadVulkanBuffer(const void* data, uint32 size, uint32 offset, VkDeviceMemory memory) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        void* mapped;
        vkMapMemory(logicalDevice, memory, offset, size, 0, &mapped);
        memcpy(mapped, data, size);
        vkUnmapMemory(logicalDevice, memory);
        return mapped;
    }

    void DestroyVulkanBuffer(VkBuffer* buffer, VkDeviceMemory* memory) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        vkDestroyBuffer(logicalDevice, *buffer, VulkanRendererAPI::GetAllocator());
        vkFreeMemory(logicalDevice, *memory, VulkanRendererAPI::GetAllocator());

        *buffer = VK_NULL_HANDLE;
        *memory = VK_NULL_HANDLE;
    }

    void CreateVulkanImage(uint32 width, uint32 height, VkFormat format, VkImageUsageFlags usage, VkImage* outImage, VkDeviceMemory* outMemory) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        VkImageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.extent.width = width;
        info.extent.height = height;
        info.extent.depth = 1;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.format = format;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.usage = usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        info.samples = VK_SAMPLE_COUNT_1_BIT;

        VkResult result = vkCreateImage(logicalDevice, &info, VulkanRendererAPI::GetAllocator(), outImage);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image!");

        VkMemoryRequirements memReq{};
        vkGetImageMemoryRequirements(logicalDevice, *outImage, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        result = vkAllocateMemory(logicalDevice, &allocInfo, VulkanRendererAPI::GetAllocator(), outMemory);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan memory!");

        result = vkBindImageMemory(logicalDevice, *outImage, *outMemory, 0);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to bind Vulkan image to memory!");
    }

    void UploadToVulkanImage(VkImage image, uint32 width, uint32 height, const void* pixels) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        VkDeviceSize size = VkDeviceSize(width * height * 4); // RGBA8

        // Create CPU-visible staging buffer
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        CreateVulkanBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &stagingBuffer, &stagingMemory);
        UploadVulkanBuffer(pixels, size, 0, stagingMemory);

        // Transition layout to transfer dst
        TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Copy buffer to image
        CopyBufferToVulkanImage(stagingBuffer, image, width, height);

        // Transition layout to shader read
        TransitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(logicalDevice, stagingBuffer, VulkanRendererAPI::GetAllocator());
        vkFreeMemory(logicalDevice, stagingMemory, VulkanRendererAPI::GetAllocator());
    }

    VkImageView CreateVulkanImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = image;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = format;
        info.subresourceRange.aspectMask = aspectMask;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;

        VkImageView imageView;
        VkResult result = vkCreateImageView(logicalDevice, &info, VulkanRendererAPI::GetAllocator(), &imageView);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image view!");
        return imageView;
    }

    uint32 FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkPhysicalDevice physicalDevice = api.GetContext()->GetPhysicalDevice();

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & BIT(i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        CODI_CORE_ASSERT(false, "Failed to find suitable memory type for Vulkan image!");
        return 0;
    }

    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        Shared<VulkanCommandBuffer> cmdBuffer = api.BeginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {
            CODI_CORE_ASSERT(false, "Unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            cmdBuffer->GetHandle(),
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        api.EndSingleTimeCommands(cmdBuffer);
    }

    void CopyBufferToVulkanImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        Shared<VulkanCommandBuffer> cmdBuffer = api.BeginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(
            cmdBuffer->GetHandle(),
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        api.EndSingleTimeCommands(cmdBuffer);
    }


} // namespace Codi::Utils
