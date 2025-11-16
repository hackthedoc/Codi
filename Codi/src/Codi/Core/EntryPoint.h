#pragma once

#include "Codi/Core/Application.h"

#ifdef CODI_PLATFORM_WINDOWS

extern Codi::Application* Codi::CreateApplication();

int main(int argc, char** argv) {
    Codi::Log::Init();

    auto app = Codi::CreateApplication();

    app->Run();

    delete app;

    Codi::Log::Shutdown();

    return 0;
}

#endif
