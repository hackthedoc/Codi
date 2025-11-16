#include "codipch.h"
#include "Application.h"

namespace Codi {

    Application* Application::_Instance = nullptr;

    Application::Application(const ApplicationSpecification& spec) : _Specification(spec) {
        CODI_CORE_ASSERT(!_Instance, "Application already exists!");
        _Instance = this;

        // Set working directory
        if (!_Specification.WorkingDirectory.empty())
            std::filesystem::current_path(_Specification.WorkingDirectory);

        // Create window
        _Window = Window::Create(WindowSpecification(spec.Name, spec.Width, spec.Height));
        _Window->SetEventCallback(CODI_BIND_EVENT_FN(Application::OnEvent));

        // Init subsystems
        // Nothing yet

        _Running = true;
    }

    Application::~Application() {
        // Shutdown subsystems
        // Nothing yet
    }

    void Application::Run() {
        while (_Running) {
            const float64 time = _Clock.Elapsed();
            const float64 deltatime = time - _lastFrameTime;
            _lastFrameTime = time;
            
            _Window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(CODI_BIND_EVENT_FN(Application::OnWindowClosed));
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e) {
        _Running = false;
        return true;
    }

} // namespace Codi
