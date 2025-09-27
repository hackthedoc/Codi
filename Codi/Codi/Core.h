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

#ifdef CODI_ENABLE_ASSERTS
    #define CASSERT(x, ...) { if (!(x)) { CODI_ERROR("Assertion Failoed: {0}", __VA_ARGS__); __debugbreak(); } }
    #define CODI_CORE_ASSERT(x, ...) { if (!(x)) { CODI_CORE_ERROR("Assertion Failoed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define CASSERT(x, ...)
    #define CODI_CORE_ASSERT(x, ...)
#endif

#define BIT(x) 1 << x

using uint = unsigned int;
