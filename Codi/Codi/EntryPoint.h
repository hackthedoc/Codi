#pragma once

#include "Codi/Application.h"

#ifdef CODI_PLATFORM_WINDOWS

extern Codi::Application* Codi::CreateApplication();

int main(int argc, char** argv) {
    Codi::Log::Init();
    CODI_CORE_INFO("Intialized Core Log!");
    CINFO("Intialized Client Log!");

    auto app = Codi::CreateApplication();
    app->run();
    delete app;

    return 0;
}

#endif
