#include "codipch.h"
#include "VulkanGraphicsContext.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanSwapchain.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace Codi {

    namespace Utils {

        static std::string VendorIDToString(uint32 vendorID) {
            switch (vendorID) {
            case 0x10DE: return "NVIDIA";
            case 0x1002: return "AMD";
            case 0x8086: return "Intel";
            case 0x13B4: return "ARM";
            case 0x5143: return "Qualcomn";
            }

            CODI_CORE_ERROR("Unknown Vulkan VendorID '{0}'", vendorID);
            return "Other";
        }

        static std::string DeviceTypeToString(VkPhysicalDeviceType deviceType) {
            switch (deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "Discrete GPU";
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "Virtual GPU";
            case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU (software)";
            case VK_PHYSICAL_DEVICE_TYPE_OTHER: return "Other";
            }

            CODI_CORE_ERROR("Unknown Vulkan DeviceType '{0}'", (uint32)deviceType);
            return "Other";
        }

#if defined(CODI_DEBUG)
        VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT flags, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
            switch (severity) {
            default:
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                CODI_CORE_ERROR(callbackData->pMessage);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                CODI_CORE_WARN(callbackData->pMessage);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                CODI_CORE_INFO(callbackData->pMessage);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
                CODI_CORE_TRACE(callbackData->pMessage);
                break;
            }
            }

            return VK_FALSE;
        }
#endif

    } // namespace Utils

    VulkanGraphicsContext::VulkanGraphicsContext(void* windowHandle) : _WindowHandle(windowHandle) {}

    void VulkanGraphicsContext::Create(const VulkanDeviceRequirements& requirements) {
        CODI_CORE_INFO("Initializing Vulkan Grapghics Context...");

        CreateInstance();
#if defined(CODI_DEBUG)
        SetupDebugMessenger();
#endif

        CreateSurface();

        PickPhysicalDevice(requirements);
        VulkanSwapchain::SetSupport(_PhysicalDevice, _Surface);
        DetectDepthFormat();
        CreateLogicalDevice(requirements);

        LogDeviceInfo();

        CODI_CORE_INFO("Vulkan Graphics Context initialized successfully.");
    }

    void VulkanGraphicsContext::Destroy() {
        CODI_CORE_INFO("Destroying Vulkan Device...");
        vkDestroyDevice(_LogicalDevice, VulkanRendererAPI::GetAllocator());
        _PhysicalDevice = VK_NULL_HANDLE;
        _LogicalDevice = VK_NULL_HANDLE;

        CODI_CORE_INFO("Destroying Vulkan surface...");
        vkDestroySurfaceKHR(_Instance, _Surface, VulkanRendererAPI::GetAllocator());
        _Surface = VK_NULL_HANDLE;

#if defined(CODI_DEBUG)
        CODI_CORE_INFO("Destroying Vulkan debugger...");
        if (_DebugMessenger) {
            PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_Instance, "vkDestroyDebugUtilsMessengerEXT");
            func(_Instance, _DebugMessenger, VulkanRendererAPI::GetAllocator());
            _DebugMessenger = VK_NULL_HANDLE;
        }
#endif

        CODI_CORE_INFO("Destroying Vulkan instance...");
        vkDestroyInstance(_Instance, VulkanRendererAPI::GetAllocator());
        _Instance = VK_NULL_HANDLE;
    }

    void VulkanGraphicsContext::WaitDeviceIdle() const {
        vkDeviceWaitIdle(_LogicalDevice);
    }

    void VulkanGraphicsContext::CreateInstance() {
        CODI_CORE_INFO("Creating Vulkan instance...");

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Codi Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Codi";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        // Setup required extentions
        uint32 extCount = 0;
        const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&extCount);
        CODI_CORE_ASSERT(sdlExtensions, "SDL failed to query Vulkan instance extensions.");

        std::vector<const char*> reqExtentions(sdlExtensions, sdlExtensions + extCount);

#ifdef CODI_DEBUG
        reqExtentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        CODI_CORE_TRACE("Required Extensions:");
        for (const char* ext : reqExtentions)
            CODI_CORE_TRACE("  - {0}", ext);
#endif

        // Setup validation layer
        std::vector<const char*> reqValLayers;

        // if validation should be done, get a list of the required validation layer names
        // and make sure they exist. Validation layers should only be enabled on non-release builds.
#if defined(CODI_DEBUG)
        CODI_CORE_INFO("Validation layers enabled. Enumarating...");

        // List of required validation layers
        reqValLayers.push_back("VK_LAYER_KHRONOS_validation");

        // List of available validation layers
        uint32 availableLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        // Verify all required layers are available
        for (uint32 i = 0; i < reqValLayers.size(); i++) {
            CODI_CORE_INFO("Searching for layer: {0}...", reqValLayers[i]);
            bool found = false;
            uint32 j = 0;
            while (j < availableLayerCount && !found) {
                if (std::strcmp(reqValLayers[i], availableLayers[j].layerName) == 0)
                    found = true;
                j++;
            }

            if (!found) {
                CODI_CORE_ERROR("Required validation layer is missing : {0}", reqValLayers[i]);
                CODI_ASSERT(false);
            }
        }

        CODI_CORE_INFO("All required validation layers are present.");
#endif

        // Instance Create Info
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        createInfo.enabledExtensionCount = reqExtentions.size();
        createInfo.ppEnabledExtensionNames = reqExtentions.data();

        createInfo.enabledLayerCount = reqValLayers.size();
        createInfo.ppEnabledLayerNames = reqValLayers.data();

        VkResult result = vkCreateInstance(&createInfo, VulkanRendererAPI::GetAllocator(), &_Instance);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!");

        CODI_CORE_INFO("Vulkan instance created.");
    }

    void VulkanGraphicsContext::CreateSurface() {
        CODI_CORE_INFO("Creating Vulkan surface...");

        SDL_Window* sdlWindow = static_cast<SDL_Window*>(_WindowHandle);
        bool result = SDL_Vulkan_CreateSurface(sdlWindow, _Instance, VulkanRendererAPI::GetAllocator(), &_Surface);

        CODI_CORE_ASSERT(result, "Failed to create Vulkan surface!");

        CODI_CORE_INFO("Vulkan surface created.");
    }

    void VulkanGraphicsContext::PickPhysicalDevice(const VulkanDeviceRequirements& requirements) {
        uint32 deviceCount = 0;
        vkEnumeratePhysicalDevices(_Instance, &deviceCount, nullptr);
        CODI_CORE_ASSERT(deviceCount > 0, "Failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_Instance, &deviceCount, devices.data());

        uint32 bestScore = 0;
        for (const VkPhysicalDevice& device : devices) {
            uint32 score = RatePhysicalDevice(device, requirements);
            if (score > bestScore) {
                bestScore = score;
                _PhysicalDevice = device;
            }
        }

        CODI_CORE_ASSERT(_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");

        vkGetPhysicalDeviceProperties(_PhysicalDevice, &_Properties);
        vkGetPhysicalDeviceFeatures(_PhysicalDevice, &_Features);
        vkGetPhysicalDeviceMemoryProperties(_PhysicalDevice, &_MemoryProperties);
    }

    void VulkanGraphicsContext::CreateLogicalDevice(const VulkanDeviceRequirements& requirements) {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::unordered_set<uint32> uniqueQueueFamilies = {
            (uint32)_QueueIndices.GraphicsFamily,
            (uint32)_QueueIndices.PresentFamily,
            (uint32)_QueueIndices.ComputeFamily,
            (uint32)_QueueIndices.TransferFamily,
        };

        float32 queuePriority = 1.0f;
        for (uint32 queueFamily : uniqueQueueFamilies) {
            if (queueFamily < 0) continue;
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1; // Queue enable multiple queue counts
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        if (requirements.SamplerAnisotropy)
            deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = (uint32)queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = (uint32)requirements.DeviceExtensions.size();
        createInfo.ppEnabledExtensionNames = requirements.DeviceExtensions.data();

        VkResult result = vkCreateDevice(_PhysicalDevice, &createInfo, nullptr, &_LogicalDevice);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan logical device!");

        vkGetDeviceQueue(_LogicalDevice, _QueueIndices.GraphicsFamily, 0, &_GraphicsQueue);
        vkGetDeviceQueue(_LogicalDevice, _QueueIndices.PresentFamily, 0, &_PresentQueue);
    }

    void VulkanGraphicsContext::DetectDepthFormat() {
        std::vector<VkFormat> candidates = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
        for (auto format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(_PhysicalDevice, format, &props);
            if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                _DepthFormat = format;
                return;
            }
        }
        CODI_CORE_ERROR("No supported depth format found!");
    }

    uint32 VulkanGraphicsContext::RatePhysicalDevice(VkPhysicalDevice device, const VulkanDeviceRequirements& requirements) {
        VkPhysicalDeviceProperties props;
        VkPhysicalDeviceFeatures feats;
        vkGetPhysicalDeviceProperties(device, &props);
        vkGetPhysicalDeviceFeatures(device, &feats);

        VulkanQueueFamilyIndices indices = FindQueueFamilies(device);
        if (!indices.IsComplete()) return 0;

        bool extensionsSupported = CheckDeviceExtensionSupport(device, requirements.DeviceExtensions);
        if (!extensionsSupported) return 0;

        SwapchainSupportDetails swapSupport = VulkanSwapchain::QuerySupport(device, _Surface);
        if (!swapSupport.IsAdequate()) return 0;

        uint32 score = 0;
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;

        // Prefere discrete GPUs
        if (requirements.DiscreteGPU) {
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                score += 1000;
            else
                return 0;
        }
        else {
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                score += 1000;
            else if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                score += 500;
            else
                score += 100;
        }

        // Higher max image dimension gives a higher score
        score += props.limits.maxImageDimension2D / 1024;

        if (requirements.SamplerAnisotropy && !feats.samplerAnisotropy)
            return 0;

        _QueueIndices = indices;
        return score;
    }

    VulkanQueueFamilyIndices VulkanGraphicsContext::FindQueueFamilies(VkPhysicalDevice device) const {
        VulkanQueueFamilyIndices indices;

        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

        int i = 0;
        for (const auto& queueFamily : families) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.GraphicsFamily = i;

            bool presentSupport = SDL_Vulkan_GetPresentationSupport(_Instance, device, i);
            if (presentSupport)
                indices.PresentFamily = i;

            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
                indices.ComputeFamily = i;

            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
                indices.TransferFamily = i;

            if (indices.IsComplete()) break;
            i++;
        }

        return indices;
    }

    bool VulkanGraphicsContext::CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions) const {
        uint32 extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::unordered_set<std::string> requiredExtensionsSet(requiredExtensions.begin(), requiredExtensions.end());

        for (const VkExtensionProperties& extension : availableExtensions)
            requiredExtensionsSet.erase(extension.extensionName);

        return requiredExtensionsSet.empty();
    }

    void VulkanGraphicsContext::LogDeviceInfo() const {
        std::string vendorName = Utils::VendorIDToString(_Properties.vendorID);
        std::string deviceType = Utils::DeviceTypeToString(_Properties.deviceType);

        const uint32 apiMajor = VK_VERSION_MAJOR(_Properties.apiVersion);
        const uint32 apiMinor = VK_VERSION_MINOR(_Properties.apiVersion);
        const uint32 apiPatch = VK_VERSION_PATCH(_Properties.apiVersion);

        const uint32 drvMajor = VK_VERSION_MAJOR(_Properties.driverVersion);
        const uint32 drvMinor = VK_VERSION_MINOR(_Properties.driverVersion);
        const uint32 drvPatch = VK_VERSION_PATCH(_Properties.driverVersion);

        CODI_CORE_INFO("----------------------------------------------");
        CODI_CORE_INFO(" Vulkan Device Info");
        CODI_CORE_INFO("----------------------------------------------");
        CODI_CORE_INFO("  Device:            {0}", _Properties.deviceName);
        CODI_CORE_INFO("  Vendor:            {0} (0x{1:X})", vendorName, _Properties.vendorID);
        CODI_CORE_INFO("  Device Type:       {0}", deviceType);
        CODI_CORE_INFO("  Device ID:         0x{0:X}", _Properties.deviceID);
        CODI_CORE_INFO("  Driver Version:    {0}.{1}.{2}", drvMajor, drvMinor, drvPatch);
        CODI_CORE_INFO("  Vulkan API:        {0}.{1}.{2}", apiMajor, apiMinor, apiPatch);

        CODI_CORE_INFO("----------------------------------------------");
        CODI_CORE_INFO(" Features & Limits");
        CODI_CORE_INFO("----------------------------------------------");
        CODI_CORE_INFO("  Max 2D Image Size:   {0}x{0}", _Properties.limits.maxImageDimension2D);
        CODI_CORE_INFO("  Max Uniform Buffers: {0}", _Properties.limits.maxUniformBufferRange);
        CODI_CORE_INFO("  Max Samplers:        {0}", _Properties.limits.maxSamplerAllocationCount);
        CODI_CORE_INFO("  Anisotropy:          {0}", _Features.samplerAnisotropy ? "Supported" : "Not Supported");
        CODI_CORE_INFO("  Geometry Shader:     {0}", _Features.geometryShader ? "Supported" : "Not Supported");

        CODI_CORE_INFO("----------------------------------------------");
        CODI_CORE_INFO(" Queue Families");
        CODI_CORE_INFO("----------------------------------------------");
        CODI_CORE_INFO("  Graphics Queue: {0}", _QueueIndices.GraphicsFamily);
        CODI_CORE_INFO("  Present Queue:  {0}", _QueueIndices.PresentFamily);
        if (_QueueIndices.ComputeFamily != _QueueIndices.GraphicsFamily)
            CODI_CORE_INFO("  Compute Queue:  {0}", _QueueIndices.ComputeFamily);
        if (_QueueIndices.TransferFamily != _QueueIndices.GraphicsFamily)
            CODI_CORE_INFO("  Transfer Queue: {0}", _QueueIndices.TransferFamily);
    }

#if defined(CODI_DEBUG)
    void VulkanGraphicsContext::SetupDebugMessenger() {
        CODI_CORE_INFO("Setting up Vulkan debugger...");

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugCreateInfo.pfnUserCallback = Utils::VKDebugCallback;

        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_Instance, "vkCreateDebugUtilsMessengerEXT");
        CODI_CORE_ASSERT(func, "Failed to create debug messenger!");
        func(_Instance, &debugCreateInfo, VulkanRendererAPI::GetAllocator(), &_DebugMessenger);

        CODI_CORE_INFO("Vulkan debugger created.");
    }
#endif

} // namespace Codi
