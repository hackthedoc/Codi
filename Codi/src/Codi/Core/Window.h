#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Events/Event.h"

namespace Codi {

    struct WindowSpecification {
        std::string Title;
        int32 Width;
        int32 Height;

        WindowSpecification(std::string title = "Codi Window", int32 width = 1280, int32 height = 720)
            : Title(title)
            , Width(width)
            , Height(height)
            {}
    };

    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

    public:
        virtual ~Window() = default;

        virtual void OnUpdate() = 0;

        virtual int32 GetWidth() const = 0;
        virtual int32 GetHeight() const = 0;

        virtual inline EventCallbackFn GetEventCallback() const = 0;
        virtual inline void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual inline void* GetNativeWindow() const = 0;
        virtual inline void* GetNativeInstance() const = 0;

        static Owned<Window> Create(const WindowSpecification& spec = WindowSpecification());
    };
}
