#pragma once

#include "Codi/Window.h"

struct GLFWwindow;

namespace Codi {

class WindowsWindow : public Window {
public:
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow();

    void onUpdate() override;

    inline uint getWidth() const override { return _data.w; }
    inline uint getHeight() const override { return _data.h; }

    // Window Attributes
    inline void setEventCallback(const EventCallbackFn& callback) override { _data.eventCallback = callback; }
    void setVSync(bool enabled) override;
    bool isVSync() const override;

private:
    virtual void init(const WindowProps& props);
    virtual void shutdown();

    GLFWwindow* _window;

    struct WindowData {
        std::string title;
        uint w;
        uint h;
        bool vsync;
        EventCallbackFn eventCallback;
    };
    WindowData _data;
};

}
