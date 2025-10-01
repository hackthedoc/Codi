#include "cdpch.h"
#include "Application.h"

#include "Codi/Renderer/Renderer.h"

// Temporary
#include "GLFW/glfw3.h"

namespace Codi {

Application* Application::_instance = nullptr;

Application::Application(const std::string& name) {
    CODI_PROFILE_FUNCTION();

    CODI_CORE_ASSERT(!_instance, "Application already exists!");
    _instance = this;

    _window = Window::Create(WindowProps(name));
    _window->setEventCallback(CODI_BIND_EVENT_FN(Application::onEvent));
    
    Renderer::Init();

    _imGuiLayer = new ImGuiLayer();
    pushOverlay(_imGuiLayer);    
}

Application::~Application() {
    CODI_PROFILE_FUNCTION();
    
    Renderer::Shutdown();
}

void Application::run() {    
    CODI_PROFILE_FUNCTION();
    
    while (_running) {
        CODI_PROFILE_SCOPE("RunLoop");
        
        float time = (float)glfwGetTime(); // TODO: Platform::GetTime()
        DeltaTime timestep = time - _lastFrameTime;
        _lastFrameTime = time;

        if (!_minimized) {
            {
                CODI_PROFILE_SCOPE("LayerStack onUpdate");

                for (Layer* l : _layerStack)
                l->onUpdate(timestep);
            }

            _imGuiLayer->begin();
            {
                CODI_PROFILE_SCOPE("LayerStack onImGuiRender");

                for (Layer* l : _layerStack)
                l->onImGuiRender();
            }
            _imGuiLayer->end();
        }
        
        _window->onUpdate();
    }
}

void Application::onEvent(Event& e) {
    CODI_PROFILE_FUNCTION();
    
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(CODI_BIND_EVENT_FN(Application::onWindowClosed));
    dispatcher.dispatch<WindowResizeEvent>(CODI_BIND_EVENT_FN(Application::onWindowResize));

    for (std::vector<Layer*>::iterator it = _layerStack.end(); it != _layerStack.begin();) {
        if (e.isHandled()) break;
        (*--it)->onEvent(e);
    }
}

void Application::pushLayer(Layer* layer) {
    CODI_PROFILE_FUNCTION();
    
    _layerStack.pushLayer(layer);
    layer->onAttach();
}

void Application::pushOverlay(Layer* overlay) {
    CODI_PROFILE_FUNCTION();
    
    _layerStack.pushLayer(overlay);
    overlay->onAttach();
}

bool Application::onWindowClosed(WindowCloseEvent& e) {
    _running = false;
    return true;
}

bool Application::onWindowResize(WindowResizeEvent& e) {
    CODI_PROFILE_FUNCTION();
    
    _minimized = e.getWidth() == 0 || e.getHeight() == 0;
    Renderer::OnWindowResize(e.getWidth(), e.getHeight());
    return false;
}

}