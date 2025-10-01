#pragma once

#include "cdpch.h"

#include "Codi/Core/Core.h"
#include "Codi/Events/Event.h"

namespace Codi {

struct WindowProps {
    std::string title;
    uint32_t width;
    uint32_t height;

    WindowProps(const std::string& t = "Codi Engine",
                uint32_t w = 1280,
                uint32_t h = 720)
        : title(t), width(w), height(h)
    {}
};


/// @brief Interface representing a desktop system based Window
class CAPI Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() {}

    virtual void onUpdate() = 0;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;

    // Window Attributes
    virtual void setEventCallback(const EventCallbackFn& callback) = 0;
    virtual void setVSync(bool enabled) = 0;
    virtual bool isVSync() const = 0;
    
    virtual void* getNativeWindow() const = 0;

    static Scope<Window> Create(const WindowProps& props = WindowProps());
};

}
