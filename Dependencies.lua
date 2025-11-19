-- Core Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["glm"]           = "%{wks.location}/Codi/vendor/glm"
IncludeDir["spdlog"]        = "%{wks.location}/Codi/vendor/spdlog/include"
IncludeDir["SDL"]           = "%{wks.location}/Codi/vendor/SDL/include"
IncludeDir["VulkanSDK"]     = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["SDL"] = "%{wks.location}/Codi/vendor/SDL/lib/x64"
LibraryDir["VulkanSDK"]     = "%{VULKAN_SDK}/Lib"

Library = {}
Library["SDL"]           = "%{LibraryDir.SDL}/SDL3.lib"
Library["Vulkan"]           = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

Library["ShaderC_Debug"]            = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"]        = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"]   = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"]        = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"]          = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"]      = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
