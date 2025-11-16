#pragma once

#include "Codi/Core/Window.h"

#include <SDL3/SDL.h>

namespace Codi {

    class WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowSpecification& spec);
        ~WindowsWindow();

        virtual void OnUpdate() override;

        virtual int32 GetWidth() const override { return _Data.Width; }
        virtual int32 GetHeight() const override { return _Data.Height; }

        virtual inline EventCallbackFn GetEventCallback() const { return _Data.EventCallback; }
        virtual inline void SetEventCallback(const EventCallbackFn& callback) override { _Data.EventCallback = callback; }

        // Return the SDL_Window*
        virtual inline void* GetNativeWindow() const override { return _Handle; }
        // Returns the underlying instance. For SDL this is just the window pointer.
        virtual inline void* GetNativeInstance() const override { return _Handle; }

    private:
        void Init(const WindowSpecification& spec);
        void Shutdown();

    private:
        struct Metadata {
            std::string Title = "Codi Windows Window";
            int32 Width = 1280;
            int32 Height = 720;
            EventCallbackFn EventCallback;
        };
        Metadata _Data{};

        SDL_Window* _Handle = nullptr;
    };

} // namespace Codi
