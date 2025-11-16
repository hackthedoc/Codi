#pragma once

namespace Codi {
    void LogTest();
}

#ifdef CODI_PLATFORM_WINDOWS

extern void Codi::LogTest();

int main(int argc, char** argv) {
    Codi::Log::Init();

    Codi::LogTest();

    Codi::Log::Shutdown();

    return 0;
}

#endif
