#pragma once

#ifdef CODI_PLATFORM_WINDOWS
    #ifdef CODI_BUILD_DLL
    #define CAPI __declspec(dllexport)
    #else
    #define CAPI __declspec(dllimport)
    #endif
#else
    #define CAPI
    #error "CODI only supports Windows!"
#endif

#define BIT(x) 1 << x

using uint = unsigned int;
