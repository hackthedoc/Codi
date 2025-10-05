#pragma once

#include "Codi/Core/Window.h"
#include "Codi/Renderer/GraphicsContext.h"

class GLFWwindow;

namespace Codi {

class WindowsWindow : public Window {
public:
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow();

    void onUpdate() override;

    inline uint32_t getWidth() const override { return _data.w; }
    inline uint32_t getHeight() const override { return _data.h; }

    // Window Attributes
    inline void setEventCallback(const EventCallbackFn& callback) override { _data.eventCallback = callback; }
    void setVSync(bool enabled) override;
    bool isVSync() const override;

    inline void* getNativeWindow() const override { return _window; }
private:
    virtual void init(const WindowProps& props);
    virtual void shutdown();

    GLFWwindow* _window;
    GraphicsContext* _context;

    struct WindowData {
        std::string title;
        uint32_t w;
        uint32_t h;
        bool vsync;
        EventCallbackFn eventCallback;
    };
    WindowData _data;
};

}
