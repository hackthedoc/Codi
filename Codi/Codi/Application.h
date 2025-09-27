#pragma once

#include "Core.h"
#include "Window.h"

#include "Events/ApplicationEvent.h"

namespace Codi {

class CAPI Application {
public:
    Application();
    virtual ~Application();

    void run();

    void onEvent(Event& e);
private:
    bool onWindowClosed(WindowCloseEvent& e);

    std::unique_ptr<Window> _window;
    bool _running = true;
};

// To be defined in CLIENT
Application* CreateApplication();

}
