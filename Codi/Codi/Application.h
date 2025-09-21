#pragma once

#include "Core.h"

namespace Codi {
    class CAPI Application {
    public:
        Application();
        virtual ~Application();
        void Run();
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}
