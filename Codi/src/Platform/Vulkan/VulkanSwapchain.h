#pragma once

#include "Codi/Core/Core.h"

#include "Platform/Vulkan/VulkanImage.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Codi {

    class VulkanGraphicsContext;

    enum class VulkanSwapResult {
        Success,
        OutOfDate,
        SubOptimal,
        Error
    };

    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities{};
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;

        bool IsAdequate() const {
            return !Formats.empty() && !PresentModes.empty();
        }
    };

    class VulkanSwapchain {
    public:
        VulkanSwapchain() = default;
        VulkanSwapchain(VulkanGraphicsContext* context);
        ~VulkanSwapchain() = default;

        void Create(uint32 width, uint32 height);
        void Recreate(uint32 width, uint32 height);
        void Destroy();

        VulkanSwapResult AcquireNextImage(VkSemaphore imageAvailableSemaphore, VkFence fence, uint64 timeout);
        VulkanSwapResult Present(VkSemaphore renderFinishedSemaphore);

        inline VkSwapchainKHR GetHandle() const { return _Handle; }

        inline VkExtent2D GetExtent() const { return _Extent; }
        inline VkFormat GetImageFormat() const { return _ImageFormat.format; }

        inline uint32 GetImageCount() const { return (uint32)_Images.size(); }
        inline VkImageView GetFrameImageView(uint32 index) const { return _ImageViews[index]; }
        inline Shared<VulkanImage> GetDepthAttachment() const { return _DepthAttachment; }

        inline uint32 GetCurrentImageIndex() const { return _CurrentImageIndex; }
        inline uint32 GetMaxFramesInFlight() const { return _MaxFramesInFlight; }

        static SwapchainSupportDetails QuerySupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        static void SetSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) { _SwapchainSupportDetails = QuerySupport(physicalDevice, surface); }

    private:
        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32 width, uint32 height);

    private:
        VulkanGraphicsContext* _Context = nullptr;
        VkSwapchainKHR _Handle = VK_NULL_HANDLE;

        VkExtent2D _Extent{};
        VkSurfaceFormatKHR _ImageFormat{};
        VkPresentModeKHR _PresentMode{};

        std::vector<VkImage> _Images;
        std::vector<VkImageView> _ImageViews;
        Shared<VulkanImage> _DepthAttachment;

        uint32 _CurrentImageIndex = 0;
        uint8 _MaxFramesInFlight = 2;

        static SwapchainSupportDetails _SwapchainSupportDetails;
    };

} // namespace Codi
