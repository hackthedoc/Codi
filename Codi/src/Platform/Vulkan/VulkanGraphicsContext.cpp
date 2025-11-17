#include "codipch.h"
#include "VulkanGraphicsContext.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace Codi {

    namespace Utils {

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

    void VulkanGraphicsContext::Create() {
        CODI_CORE_INFO("Initializing Vulkan Grapghics Context...");

        CreateInstance();
#if defined(CODI_DEBUG)
        SetupDebugMessenger();
#endif

        CreateSurface();

        CODI_CORE_INFO("Vulkan Graphics Context initialized successfully.");
    }

    void VulkanGraphicsContext::Destroy() {
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

        std::vector<const char*> reqExtentions;
        reqExtentions.reserve(extCount);
        for (uint32 i = 0; i < extCount; i++)
            reqExtentions.push_back(sdlExtensions[i]);

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
