#pragma once

#include "Core.h"

#include "Window.h"
#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Codi {

class CAPI Application {
public:
    Application();
    virtual ~Application();

    void run();

    void onEvent(Event& e);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);
private:
    bool onWindowClosed(WindowCloseEvent& e);

    std::unique_ptr<Window> _window;
    bool _running = true;
    LayerStack _layerStack;
};

// To be defined in CLIENT
Application* CreateApplication();

}
