#pragma once

#include "Codi/Core/Application.h"

#ifdef CODI_PLATFORM_WINDOWS

extern Codi::Application* Codi::CreateApplication();

int main(int argc, char** argv) {
    Codi::Log::Init();
    
    CODI_PROFILE_BEGIN_SESSION("Startup", "CodiProfile-Startup.json");
    auto app = Codi::CreateApplication();
    CODI_PROFILE_END_SESSION();

    CODI_PROFILE_BEGIN_SESSION("Runtime", "CodiProfile-Runtime.json");
    app->run();
    CODI_PROFILE_END_SESSION();

    CODI_PROFILE_BEGIN_SESSION("Shutdown", "CodiProfile-Shutdown.json");
    delete app;
    CODI_PROFILE_END_SESSION();

    return 0;
}

#endif
