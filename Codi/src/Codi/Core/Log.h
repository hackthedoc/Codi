#pragma once

#include "Codi/Core/Core.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Codi {

    class Log {
    public:
        static void Init();
        static void Shutdown();

        inline static Shared<spdlog::logger>& GetCoreLogger() { return _CoreLogger; }
        inline static Shared<spdlog::logger>& GetClientLogger() { return _ClientLogger; }

    private:
        static Shared<spdlog::logger> _CoreLogger;
        static Shared<spdlog::logger> _ClientLogger;
    };

} // namespace Codi

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
