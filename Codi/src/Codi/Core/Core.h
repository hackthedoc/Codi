#pragma once

#include <memory>

#ifdef CODI_PLATFORM_WINDOWS
    #if CODI_DYNAMIC_LINK
        #ifdef CODI_BUILD_DLL
            #define CAPI __declspec(dllexport)
        #else
            #define CAPI __declspec(dllimport)
        #endif
    #else
        #define CAPI
    #endif
#else
    #define CAPI
    #error "CODI only supports Windows!"
#endif

#ifdef CODI_DEBUG
    #define CODI_ENABLE_ASSERTS
#endif

#ifdef CODI_ENABLE_ASSERTS
    #define CASSERT(x, ...) { if (!(x)) { CODI_ERROR("Assertion Failoed: {0}", __VA_ARGS__); __debugbreak(); } }
    #define CODI_CORE_ASSERT(x, ...) { if (!(x)) { CODI_CORE_ERROR("Assertion Failoed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define CASSERT(x, ...)
    #define CODI_CORE_ASSERT(x, ...)
#endif

#define BIT(x) 1 << x

#define CODI_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

using uint = unsigned int;

namespace Codi {

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace Codi
