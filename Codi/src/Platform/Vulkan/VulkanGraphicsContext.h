#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Codi {

    struct VulkanDeviceRequirements {
        bool Graphics = true;
        bool Present = true;
        bool Compute = false;
        bool Transfer = false;
        bool SamplerAnisotropy = true;
        bool DiscreteGPU = true;

        std::vector<const char*> DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    };

    struct VulkanQueueFamilyIndices {
        int32 GraphicsFamily = -1;
        int32 PresentFamily = -1;
        int32 ComputeFamily = -1;
        int32 TransferFamily = -1;

        bool IsComplete() const {
            return GraphicsFamily >= 0 && PresentFamily >= 0;
        }
    };

    class VulkanGraphicsContext {
    public:
        VulkanGraphicsContext(void* windowHandle);
        ~VulkanGraphicsContext() = default;

        void Create(const VulkanDeviceRequirements& requirements);
        void Destroy();

        void WaitDeviceIdle() const;

        inline VkInstance GetInstance() const { return _Instance; }
        inline VkSurfaceKHR GetSurface() const { return _Surface; }

        inline VkPhysicalDevice GetPhysicalDevice() const { return _PhysicalDevice; }
        inline VkDevice GetLogicalDevice() const { return _LogicalDevice; }

        inline const VulkanQueueFamilyIndices& GetQueueIndices() const { return _QueueIndices; }
        inline const VkFormat GetDepthFormat() const { return _DepthFormat; }

        inline VkQueue GetGraphicsQueue() const { return _GraphicsQueue; }
        inline VkQueue GetPresentQueue() const { return _PresentQueue; }

        inline VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return _MemoryProperties; }

    private:
        void CreateInstance();
        void CreateSurface();
        void PickPhysicalDevice(const VulkanDeviceRequirements& requirements);
        void CreateLogicalDevice(const VulkanDeviceRequirements& requirements);
        void DetectDepthFormat();

        // --- Helpers ---
        uint32_t RatePhysicalDevice(VkPhysicalDevice device, const VulkanDeviceRequirements& requirements);
        VulkanQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions) const;
        void LogDeviceInfo() const;

#if defined(CODI_DEBUG)
        void SetupDebugMessenger();
#endif

    private:
        void* _WindowHandle = nullptr;
        VkInstance _Instance = VK_NULL_HANDLE;
        VkSurfaceKHR _Surface = VK_NULL_HANDLE;

        VkPhysicalDevice _PhysicalDevice = VK_NULL_HANDLE;
        VkDevice _LogicalDevice = VK_NULL_HANDLE;

        VulkanQueueFamilyIndices _QueueIndices;
        VkFormat _DepthFormat = VK_FORMAT_UNDEFINED;

        VkQueue _GraphicsQueue = VK_NULL_HANDLE;
        VkQueue _PresentQueue = VK_NULL_HANDLE;

        VkPhysicalDeviceProperties _Properties{};
        VkPhysicalDeviceFeatures _Features{};
        VkPhysicalDeviceMemoryProperties _MemoryProperties{};

#if defined(CODI_DEBUG)
        VkDebugUtilsMessengerEXT _DebugMessenger = VK_NULL_HANDLE;
#endif
    };

} // namespace Codi
