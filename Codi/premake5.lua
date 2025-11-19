project "Codi"
    kind "StaticLib"
    language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	pchheader "codipch.h"
	pchsource "src/codipch.cpp"

    files {
        "src/**.h",
        "src/**.cpp",

		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",

		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp",
    }

    includedirs {
        "%{wks.location}/Codi/src",

		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.SDL}",
		"%{IncludeDir.VulkanSDK}",
    }

	links {
		"ImGui",

		"%{Library.SDL}",
		"%{Library.Vulkan}",
	}

	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"
        buildoptions { "/utf-8" }

	filter "configurations:Debug"
		defines "CODI_DEBUG"
		runtime "Debug"
		symbols "on"

		links {
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}",
		}

	filter "configurations:Release"
		defines "CODI_RELEASE"
		runtime "Release"
		optimize "on"

		links {
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}",
		}

	filter "configurations:Dist"
		defines "CODI_DIST"
		runtime "Release"
		optimize "on"

		links {
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}",
		}
