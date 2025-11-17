-- Core Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"]        = "%{wks.location}/Codi/vendor/spdlog/include"
IncludeDir["SDL"]           = "%{wks.location}/Codi/vendor/SDL/include"
IncludeDir["VulkanSDK"]     = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["SDL"] = "%{wks.location}/Codi/vendor/SDL/lib/x64"
LibraryDir["VulkanSDK"]     = "%{VULKAN_SDK}/Lib"

Library = {}
Library["SDL"]           = "%{LibraryDir.SDL}/SDL3.lib"
Library["Vulkan"]           = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
