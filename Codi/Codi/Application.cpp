#include "Application.h"

namespace Codi {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application() {
    _window = std::unique_ptr<Window>(Window::Create());
    _window->setEventCallback(BIND_EVENT_FN(onEvent));
}

Application::~Application() {}

void Application::run() {
    while (_running) {
        for (Layer* l : _layerStack)
            l->onUpdate();
        _window->onUpdate();
    }
}

void Application::onEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClosed));

    CODI_CORE_TRACE("{0}", e.toString());

    for (std::vector<Layer*>::iterator it = _layerStack.end(); it != _layerStack.begin();) {
        (*--it)->onEvent(&e);
        if (e.isHandled()) break;
    }
}

void Application::pushLayer(Layer* layer) {
    _layerStack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overlay) {
    _layerStack.pushLayer(overlay);
}

bool Application::onWindowClosed(WindowCloseEvent& e) {
    _running = false;
    return true;
}

}