#pragma once

#include "Codi/Core.h"
#include "Codi/Window.h"
#include "Codi/LayerStack.h"
#include "Codi/Events/Event.h"
#include "Codi/Events/ApplicationEvent.h"
#include "Codi/ImGui/ImGuiLayer.h"

namespace Codi {

class CAPI Application {
public:
    Application();
    virtual ~Application();

    void run();

    void onEvent(Event& e);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);

    inline static Application& Get() { return *_instance; }
    inline Window& getWindow() { return *_window; }
private:
    bool onWindowClosed(WindowCloseEvent& e);

    std::unique_ptr<Window> _window;
    ImGuiLayer* _imGuiLayer;
    bool _running = true;
    LayerStack _layerStack;

    static Application* _instance;
};

// To be defined in CLIENT
Application* CreateApplication();

}
