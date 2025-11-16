#include "codipch.h"
#include "WindowsWindow.h"

#include "Codi/Events/ApplicationEvents.h"

#include "Codi/Utils/PlatformUtils.h"

namespace Codi {
    static float64 PlatformFrequency;

    Owned<Window> Window::Create(const WindowSpecification& spec) {
        return Own<WindowsWindow>(spec);
    }

    WindowsWindow::WindowsWindow(const WindowSpecification& spec) {
        Init(spec);
    }

    WindowsWindow::~WindowsWindow() {
        Shutdown();
    }

    void WindowsWindow::Init(const WindowSpecification& spec) {
        _Data.Title = spec.Title;
        _Data.Width = spec.Width;
        _Data.Height = spec.Height;

        CODI_CORE_INFO("Creating Windows window {0} ({1}, {2})", _Data.Title, _Data.Width, _Data.Height);

        if (!SDL_WasInit(SDL_INIT_VIDEO)) {
            if (!SDL_Init(SDL_INIT_VIDEO)) {
                CODI_CORE_ASSERT(false, "Failed to initialize SDL3 Video subsystem");
            }
        }

        // Create SDL3 Window
        _Handle = SDL_CreateWindow(
            _Data.Title.c_str(),
            _Data.Width, _Data.Height,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
        );

        // Clock Setup
        PlatformFrequency = 1.0 / (float64)SDL_GetPerformanceFrequency();

        CODI_CORE_ASSERT(_Handle, "Failed to create SDL window!");
    }

    void WindowsWindow::Shutdown() {
        CODI_CORE_INFO("Destroying SDL window {0}", _Data.Title);

        if (_Handle) {
            SDL_DestroyWindow(_Handle);
            _Handle = nullptr;
        }
    }

    void WindowsWindow::OnUpdate() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT: {
                WindowCloseEvent e;
                _Data.EventCallback(e);
                break;
            }
                
            }
        }
    }
    // -------------------------
    // Platform Utils
    // -------------------------

    float64 Platform::GetAbsoluteTime() {
        static const uint64 PlatformStartTicks = SDL_GetPerformanceCounter();
        uint64 currentTicks = SDL_GetPerformanceCounter();
        return (float64)(currentTicks - PlatformStartTicks) * PlatformFrequency;
    }

    void Platform::Sleep(uint32 ms) {
        SDL_Delay(ms);
    }


} // namespace Codi
