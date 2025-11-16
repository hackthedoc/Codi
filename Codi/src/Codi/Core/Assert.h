#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Core/Log.h"

#include <filesystem>

#ifdef CODI_ENABLE_ASSERTS
    #define CODI_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { CODI##type##ERROR(msg, __VA_ARGS__); CODI_DEBUGBREAK(); } }
    #define CODI_INTERNAL_ASSERT_WITH_MSG(type, check, ...) CODI_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
    #define CODI_INTERNAL_ASSERT_NO_MSG(type, check) CODI_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", CODI_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

    #define CODI_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
    #define CODI_INTERNAL_ASSERT_GET_MACRO(...) CODI_EXPAND_MACRO( CODI_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, CODI_INTERNAL_ASSERT_WITH_MSG, CODI_INTERNAL_ASSERT_NO_MSG) )

    #define CODI_ASSERT(...) CODI_EXPAND_MACRO( CODI_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
    #define CODI_CORE_ASSERT(...) CODI_EXPAND_MACRO( CODI_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
    #define CASSERT(...)
    #define CODI_CORE_ASSERT(...)
#endif
