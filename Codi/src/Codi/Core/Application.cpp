#include "codipch.h"
#include "Application.h"

#include "Codi/Renderer/Renderer.h"

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
        Renderer::Init();

        _ImGuiLayer = new ImGuiLayer();
        PushOverlay(_ImGuiLayer);

        _Running = true;
    }

    Application::~Application() {
        _ImGuiLayer->OnDetach();

        // Shutdown subsystems
        Renderer::Shutdown();
    }

    void Application::Run() {
        while (_Running) {
            const float64 time = _Clock.Elapsed();
            const float64 deltatime = time - _lastFrameTime;
            _lastFrameTime = time;

            if (!_Minimized) {
                Renderer::BeginFrame();

                for (Layer* l : _LayerStack)
                    l->OnUpdate(deltatime);

                _ImGuiLayer->Begin();
                {
                    for (Layer* l : _LayerStack)
                        l->OnImGuiRender();
                }
                _ImGuiLayer->End();

                Renderer::EndFrame();
            }

            _Window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(CODI_BIND_EVENT_FN(Application::OnWindowClosed));
        dispatcher.Dispatch<WindowResizeEvent>(CODI_BIND_EVENT_FN(Application::OnWindowResize));

        _ImGuiLayer->OnEvent(e);

        for (std::vector<Layer*>::iterator it = _LayerStack.end(); it != _LayerStack.begin();) {
            if (e.IsHandled()) break;
            (*--it)->OnEvent(e);
        }
    }

    void Application::PushLayer(Layer* layer) {
        _LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay) {
        _LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e) {
        _Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e) {
        // window minimized, ignore
        if (e.GetWidth() == 0 || e.GetHeight() == 0) {
            _Minimized = true;
            return false;
        }

        _Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
        return false;
    }

} // namespace Codi
