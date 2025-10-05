#pragma once

#include <memory>

#if defined(CODI_DEBUG)
    #if defined(CODI_PLATFORM_WINDOWS)
        #define CODI_DEBUGBREAK() __debugbreak()
    #elif defined(CODI_PLATFORM_LINUX)
        #include <signal.h>
        #define CODI_DEBUGBREAK() raise(SIGTRAP)
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif

    #define CODI_ENABLE_ASSERTS
#else
    #define CODI_DEBUGBREAK()
#endif

#define CODI_EXPAND_MACRO(x) x
#define CODI_STRINGIFY_MACRO(x) #x

#define BIT(x) 1 << x

#define CODI_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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

#include "Codi/Core/Assert.h"
