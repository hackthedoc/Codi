#include "cdpch.h"
#include "Application.h"

#include "Codi/Renderer/Renderer.h"

namespace Codi {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::_instance = nullptr;

Application::Application() {
    CODI_CORE_ASSERT(!_instance, "Application already exists!");
    _instance = this;

    _window = std::unique_ptr<Window>(Window::Create());
    _window->setEventCallback(BIND_EVENT_FN(onEvent));

    _imGuiLayer = new ImGuiLayer();
    pushOverlay(_imGuiLayer);    
}

Application::~Application() {}

void Application::run() {
    while (_running) {
        for (Layer* l : _layerStack)
            l->onUpdate();
        
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

    CODI_CORE_TRACE("{0}", e.toString());

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