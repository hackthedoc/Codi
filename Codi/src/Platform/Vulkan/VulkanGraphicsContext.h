#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Codi {

    class VulkanGraphicsContext {
    public:
        VulkanGraphicsContext(void* windowHandle);
        ~VulkanGraphicsContext() = default;

        void Create();
        void Destroy();

        inline VkInstance GetInstance() const { return _Instance; }
        inline VkSurfaceKHR GetSurface() const { return _Surface; }

    private:
        void CreateInstance();
        void CreateSurface();

#if defined(CODI_DEBUG)
        void SetupDebugMessenger();
#endif

    private:
        void* _WindowHandle = nullptr;
        VkInstance _Instance = VK_NULL_HANDLE;
        VkSurfaceKHR _Surface = VK_NULL_HANDLE;

#if defined(CODI_DEBUG)
        VkDebugUtilsMessengerEXT _DebugMessenger = VK_NULL_HANDLE;
#endif
    };

} // namespace Codi
