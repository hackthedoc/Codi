#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Core/Window.h"
#include "Codi/Core/LayerStack.h"
#include "Codi/Core/DeltaTime.h"
#include "Codi/Events/Event.h"
#include "Codi/Events/ApplicationEvent.h"
#include "Codi/ImGui/ImGuiLayer.h"

namespace Codi {

class Application {
public:
    Application(const std::string& name = "Codi Application");
    virtual ~Application();

    void run();

    void onEvent(Event& e);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);

    void close() { _running = false; }

    inline ImGuiLayer* getImGuiLayer() { return _imGuiLayer; }

    inline static Application& Get() { return *_instance; }
    inline Window& getWindow() { return *_window; }
private:
    bool onWindowClosed(WindowCloseEvent& e);
    bool onWindowResize(WindowResizeEvent& e);

    Scope<Window> _window;
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
