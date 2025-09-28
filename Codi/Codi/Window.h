#pragma once

#include "cdpch.h"

#include "Core.h"
#include "Events/Event.h"

namespace Codi {

struct WindowProps {
    std::string title;
    uint width;
    uint height;

    WindowProps(const std::string& t = "Codi Engine",
                uint w = 1280,
                uint h = 720)
        : title(t), width(w), height(h)
    {}
};


/// @brief Interface representing a desktop system based Window
class CAPI Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() {}

    virtual void onUpdate() = 0;

    virtual uint getWidth() const = 0;
    virtual uint getHeight() const = 0;

    // Window Attributes
    virtual void setEventCallback(const EventCallbackFn& callback) = 0;
    virtual void setVSync(bool enabled) = 0;
    virtual bool isVSync() const = 0;
    
    virtual void* getNativeWindow() const = 0;

    static Window* Create(const WindowProps& props = WindowProps());
};

}
