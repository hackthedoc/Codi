#pragma once

#include "Codi/Core.h"
#include "Codi/Window.h"
#include "Codi/LayerStack.h"
#include "Codi/Core/DeltaTime.h"
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

    void close() { _running = false; }

    inline static Application& Get() { return *_instance; }
    inline Window& getWindow() { return *_window; }
private:
    bool onWindowClosed(WindowCloseEvent& e);
    bool onWindowResize(WindowResizeEvent& e);

    std::unique_ptr<Window> _window;
    ImGuiLayer* _imGuiLayer;
    bool _running = true;
    bool _minimized = false;
    LayerStack _layerStack;
    float _lastFrameTime = 0.0f;

    static Application* _instance;
};

// To be defined in CLIENT
Application* CreateApplication();

}
