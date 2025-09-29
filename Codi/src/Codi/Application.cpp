#include "cdpch.h"
#include "Application.h"

#include "Codi/Renderer/Renderer.h"

// Temporary
#include "GLFW/glfw3.h"

namespace Codi {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::_instance = nullptr;

Application::Application() {
    CODI_CORE_ASSERT(!_instance, "Application already exists!");
    _instance = this;

    _window = std::unique_ptr<Window>(Window::Create());
    _window->setEventCallback(BIND_EVENT_FN(onEvent));
    
    Renderer::Init();

    _imGuiLayer = new ImGuiLayer();
    pushOverlay(_imGuiLayer);    
}

Application::~Application() {}

void Application::run() {
    while (_running) {
        float time = (float)glfwGetTime(); // TODO: Platform::GetTime()
        DeltaTime timestep = time - _lastFrameTime;
        _lastFrameTime = time;

        for (Layer* l : _layerStack)
            l->onUpdate(timestep);
        
        _imGuiLayer->begin();
        for (Layer* l : _layerStack)
            l->onImGuiRender();
        _imGuiLayer->end();

        _window->onUpdate();
    }
}

void Application::onEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClosed));

    for (std::vector<Layer*>::iterator it = _layerStack.end(); it != _layerStack.begin();) {
        (*--it)->onEvent(e);
        if (e.isHandled()) break;
    }
}

void Application::pushLayer(Layer* layer) {
    _layerStack.pushLayer(layer);
    layer->onAttach();
}

void Application::pushOverlay(Layer* overlay) {
    _layerStack.pushLayer(overlay);
    overlay->onAttach();
}

bool Application::onWindowClosed(WindowCloseEvent& e) {
    _running = false;
    return true;
}

}