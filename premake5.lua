include "./vendor/premake/premake_customization/solution_items.lua"
include "./Dependencies.lua"

workspace "Codi"
    architecture "x86_64"
    startproject "Engine"

    configurations {
        "Debug",
        "Release",
        "Dist"
    }

    solution_items {
        ".editorconfig"
    }

    flags {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
    include "Codi"
group ""

group "Dependencies"
    include "vendor/premake"
group ""

group "Tools"
    include "Engine"
group ""
