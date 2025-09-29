#include "cdpch.h"
#include "WindowsWindow.h"

#include "Codi/Events/ApplicationEvent.h"
#include "Codi/Events/KeyEvent.h"
#include "Codi/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include "GLFW/glfw3.h"

namespace Codi {

static bool _GLFWInitialized = false;

static void GLFWErrorCallback(int err, const char* msg) {
    CODI_CORE_ERROR("GLFW Error ({0}): {1}", err ,msg);
}

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

        glfwSetErrorCallback(GLFWErrorCallback);
        _GLFWInitialized = true;
    }

    _window = glfwCreateWindow((int)_data.w, (int)_data.h, _data.title.c_str(), nullptr, nullptr);
    
    _context = new OpenGLContext(_window);
    _context->init();

    glViewport(0, 0, _data.w, _data.h);

    glfwSetWindowUserPointer(_window, &_data);
    setVSync(true);

    // set glfw callbacks
    
    glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int w, int h) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.w = w;
        data.h = h;

        glViewport(0, 0, w, h);

        WindowResizeEvent event(w, h);
        data.eventCallback(event);
    });
    
    glfwSetWindowCloseCallback(_window, [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        WindowCloseEvent event;
        data.eventCallback(event);
    });
    
    glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action) {
        case GLFW_PRESS:
        {
            KeyPressedEvent event(key, 0);
            data.eventCallback(event);
            break;
        }
        case GLFW_RELEASE:
        {
            KeyReleasedEvent event(key);
            data.eventCallback(event);
            break;
        }
        case GLFW_REPEAT:
        {
            KeyPressedEvent event(key, 1);
            data.eventCallback(event);
            break;
        }
        }
    });
    
    glfwSetCharCallback(_window, [](GLFWwindow* window, uint keycode) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);\

        KeyTypedEvent event(keycode);
        data.eventCallback(event);
    });
    
    glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action) {
        case GLFW_PRESS:
        {
            MouseButtonPressedEvent event(button);
            data.eventCallback(event);
            break;
        }
        case GLFW_RELEASE:
        {
            MouseButtonReleasedEvent event(button);
            data.eventCallback(event);
            break;
        }
        }
    });
    
    glfwSetScrollCallback(_window, [](GLFWwindow* window, double xOffset, double yOffset) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        data.eventCallback(event);
    });
    
    glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xPos, double yPos) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseMovedEvent event((float)xPos, (float)yPos);
        data.eventCallback(event);
    });
}

void WindowsWindow::shutdown() {
    glfwDestroyWindow(_window);
}

void WindowsWindow::onUpdate() {
    glfwPollEvents();
    _context->swapBuffers();
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