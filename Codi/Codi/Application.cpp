#include "Application.h"

#include "Events/ApplicationEvent.h"

namespace Codi {
    Application::Application() {
        _window = std::unique_ptr<Window>(Window::Create());
    }

    Application::~Application() {}

    void Application::Run() {
        while (_running) {
            _window->onUpdate();
        }
    }
}