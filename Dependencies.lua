-- Core Dependencies

IncludeDir = {}
IncludeDir["spdlog"]        = "%{wks.location}/Codi/vendor/spdlog/include"
IncludeDir["SDL"]           = "%{wks.location}/Codi/vendor/SDL/include"

LibraryDir = {}

LibraryDir["SDL"] = "%{wks.location}/Codi/vendor/SDL/lib/x64"

Library = {}

Library["SDL"]           = "%{LibraryDir.SDL}/SDL3.lib"