#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Core/Clock.h"
#include "Codi/Core/Window.h"
#include "Codi/Events/ApplicationEvents.h"

namespace Codi {

    struct ApplicationSpecification {
        std::string Name = "Codi Application";
        int32 Width = 1280;
        int32 Height = 720;
        std::string WorkingDirectory = "./";
    };

    class Application {
    public:
        Application(const ApplicationSpecification& specification);
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void Close() { _Running = false; }

        inline Window* GetWindow() { return _Window.get(); }

        inline static Application& Get() { return *_Instance; }

        const ApplicationSpecification& GetSpecification() const { return _Specification; }

    private:
        bool OnWindowClosed(WindowCloseEvent& e);

    private:
        ApplicationSpecification _Specification;

        Owned<Window> _Window;
        bool _Running = false;
        Clock _Clock;
        float64 _lastFrameTime = 0;

        static Application* _Instance;
    };

    // To be defined in CLIENT
    Application* CreateApplication();

} // namespace Codi
