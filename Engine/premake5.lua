project "Engine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

    
    files {
        "src/**.h",
        "src/**.cpp"
    }
    
    includedirs {
        "%{wks.location}/Engine/src",
        "%{wks.location}/Codi/src",
        "%{wks.location}/Codi/vendor",
        
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.spdlog}",
    }

    links {
        "Codi"
    }

    filter "system:windows"
        systemversion "latest"
        buildoptions { "/utf-8" }

    filter "configurations:Debug"
        defines { "CODI_DEBUG" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "CODI_RELEASE" }
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines { "CODI_DIST" }
        runtime "Release"
        optimize "on"
