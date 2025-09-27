#include "WindowsWindow.h"

namespace Codi {

static bool _GLFWInitialized = false;

Window* Window::Create(const WindowProps& props) {
    return new WindowsWindow(props);
}

WindowsWindow::WindowsWindow(const WindowProps& props) {
    init(props);
}

WindowsWindow::~WindowsWindow() {
    shutdown();
}

void WindowsWindow::init(const WindowProps& props) {
    _data.title = props.title;
    _data.w = props.width;
    _data.h = props.height;

    CODI_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

    if (!_GLFWInitialized) {
        int success = glfwInit();
        CODI_CORE_ASSERT(success, "Could not initialized GLFW!");

        _GLFWInitialized = true;
    }

    _window = glfwCreateWindow((int)_data.w, (int)_data.h, _data.title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(_window);
    glfwSetWindowUserPointer(_window, &_data);
    setVSync(true);
}

void WindowsWindow::shutdown() {
    glfwDestroyWindow(_window);
}

void WindowsWindow::onUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(_window);
}

void WindowsWindow::setVSync(bool enabled) {
    if (enabled) glfwSwapInterval(1);
    else glfwSwapInterval(0);

    _data.vsync = enabled;
}

bool WindowsWindow::isVSync () const {
    return _data.vsync;
}

}