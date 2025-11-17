#include "codipch.h"
#include "VulkanSwapchain.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    SwapchainSupportDetails VulkanSwapchain::_SwapchainSupportDetails{};

    VulkanSwapchain::VulkanSwapchain(VulkanGraphicsContext* context)
        : _Context(context)
        {}

    void VulkanSwapchain::Create(uint32 width, uint32 height) {
        CODI_CORE_INFO("Creating Vulkan SwapChain...");

        _MaxFramesInFlight = 2;

        _ImageFormat = ChooseSurfaceFormat(_SwapchainSupportDetails.Formats);
        _PresentMode = ChoosePresentMode(_SwapchainSupportDetails.PresentModes);
        _Extent = ChooseExtent(_SwapchainSupportDetails.Capabilities, width, height);

        uint32 imageCount = _SwapchainSupportDetails.Capabilities.minImageCount + 1;
        if (_SwapchainSupportDetails.Capabilities.maxImageCount > 0 && imageCount > _SwapchainSupportDetails.Capabilities.maxImageCount)
            imageCount = _SwapchainSupportDetails.Capabilities.maxImageCount;

        // Swapchain create info
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _Context->GetSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = _ImageFormat.format;
        createInfo.imageColorSpace = _ImageFormat.colorSpace;
        createInfo.imageExtent = _Extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Setup the queue family indeces
        const VulkanQueueFamilyIndices& indices = _Context->GetQueueIndices();
        if (indices.GraphicsFamily != indices.PresentFamily) {
            uint32 queueFamilyIndices[] = {
                (uint32)indices.GraphicsFamily,
                (uint32)indices.PresentFamily
            };
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = _SwapchainSupportDetails.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = _PresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = _Handle;

        VkResult result = vkCreateSwapchainKHR(_Context->GetLogicalDevice(), &createInfo, VulkanRendererAPI::GetAllocator(), &_Handle);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swapchain!");

        // images
        vkGetSwapchainImagesKHR(_Context->GetLogicalDevice(), _Handle, &imageCount, nullptr);
        _Images.resize(imageCount);
        vkGetSwapchainImagesKHR(_Context->GetLogicalDevice(), _Handle, &imageCount, _Images.data());

        // views
        _ImageViews.resize(imageCount);
        for (uint32 i = 0; i < imageCount; i++) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = _Images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = _ImageFormat.format;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            result = vkCreateImageView(_Context->GetLogicalDevice(), &viewInfo, VulkanRendererAPI::GetAllocator(), &_ImageViews[i]);
            CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create swapchain image view!");
        }

        // depth image and view
        VulkanImageSpecification depthSpec{};
        depthSpec.Width = _Extent.width;
        depthSpec.Height = _Extent.height;
        depthSpec.Format = _Context->GetDepthFormat();
        depthSpec.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthSpec.MemoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        depthSpec.CreateView = true;
        depthSpec.AspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthSpec.Type = VulkanImageType::DepthAttachment;
        _DepthAttachment = Share<VulkanImage>(depthSpec);

        CODI_CORE_INFO("Vulkan SwapChain created with {0} images.", imageCount);
    }

    void VulkanSwapchain::Recreate(uint32 width, uint32 height) {
        Destroy();
        Create(width, height);
    }

    void VulkanSwapchain::Destroy() {
        CODI_CORE_INFO("Destroying Vulkan swapchain...");

        _DepthAttachment->Destroy();
        _DepthAttachment.reset();

        for (auto view : _ImageViews)
            vkDestroyImageView(_Context->GetLogicalDevice(), view, VulkanRendererAPI::GetAllocator());
        _ImageViews.clear();
        _Images.clear();

        vkDestroySwapchainKHR(_Context->GetLogicalDevice(), _Handle, VulkanRendererAPI::GetAllocator());
        _Handle = VK_NULL_HANDLE;
    }

    VulkanSwapResult VulkanSwapchain::AcquireNextImage(VkSemaphore imageAvailableSemaphore, VkFence fence, uint64 timeout) {
        VkResult result = vkAcquireNextImageKHR(
            _Context->GetLogicalDevice(),
            _Handle,
            timeout,
            imageAvailableSemaphore,
            fence,
            &_CurrentImageIndex
        );

        switch (result) {
            case VK_SUCCESS: return VulkanSwapResult::Success;
            case VK_SUBOPTIMAL_KHR: return VulkanSwapResult::SubOptimal;
            case VK_ERROR_OUT_OF_DATE_KHR: return VulkanSwapResult::OutOfDate;
        }
        return VulkanSwapResult::Error;
    }

    VulkanSwapResult VulkanSwapchain::Present(VkSemaphore renderFinishedSemaphore) {
        // Return the image to the swapchain for presentation
        VkPresentInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &renderFinishedSemaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &_Handle;
        info.pImageIndices = &_CurrentImageIndex;

        VkResult result = vkQueuePresentKHR(_Context->GetPresentQueue(), &info);

        switch (result) {
        case VK_SUCCESS: return VulkanSwapResult::Success;
        case VK_SUBOPTIMAL_KHR: return VulkanSwapResult::SubOptimal;
        case VK_ERROR_OUT_OF_DATE_KHR: return VulkanSwapResult::OutOfDate;
        }
        return VulkanSwapResult::Error;
    }

    VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const VkSurfaceFormatKHR& format : availableFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
                return format;
        }

        // Default to the first found format
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& mode : availablePresentModes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
        }
        // always supported
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32 width, uint32 height) {
        if (capabilities.currentExtent.width != UINT32_MAX)
            return capabilities.currentExtent;

        VkExtent2D actualExtent = { width, height };

        VkExtent2D min = capabilities.minImageExtent;
        VkExtent2D max = capabilities.maxImageExtent;

        actualExtent.width = std::clamp(width, min.width, max.width);
        actualExtent.height = std::clamp(height, min.height, max.height);

        return actualExtent;
    }

    SwapchainSupportDetails VulkanSwapchain::QuerySupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        SwapchainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.Capabilities);

        uint32 count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
        if (count > 0) {
            details.Formats.resize(count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, details.Formats.data());
        }

        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
        if (count > 0) {
            details.PresentModes.resize(count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, details.PresentModes.data());
        }

        return details;
    }

} // namespace Codi
