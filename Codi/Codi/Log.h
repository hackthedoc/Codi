#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Codi {
    class CAPI Log {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return _CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return _ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> _CoreLogger;
        static std::shared_ptr<spdlog::logger> _ClientLogger;
    };
}

// Core Log Macros
#define CODI_CORE_TRACE(...) ::Codi::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CODI_CORE_INFO(...) ::Codi::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CODI_CORE_WARN(...) ::Codi::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CODI_CORE_ERROR(...) ::Codi::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CODI_CORE_CRITICAL(...) ::Codi::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log Macros
#define CODI_TRACE(...) ::Codi::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CODI_INFO(...) ::Codi::Log::GetClientLogger()->info(__VA_ARGS__)
#define CODI_WARN(...) ::Codi::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CODI_ERROR(...) ::Codi::Log::GetClientLogger()->error(__VA_ARGS__)
#define CODI_CRITICAL(...) ::Codi::Log::GetClientLogger()->critical(__VA_ARGS__)
