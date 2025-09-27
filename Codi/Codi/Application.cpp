#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Log.h"

namespace Codi {
    Application::Application() {}

    Application::~Application() {}

    void Application::Run() {
        WindowResizeEvent e(1280, 720);
        CODI_TRACE(e.toString());

        while (true) {}
    }
}