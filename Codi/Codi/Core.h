#pragma once

#ifdef CODI_PLATFORM_WINDOWS
    #ifdef CODI_BUILD_DLL
    #define CAPI __declspec(dllexport)
    #else
    #define CAPI __declspec(dllimport)
    #endif
#else
    #error "CODI only supports Windows!"
#endif
