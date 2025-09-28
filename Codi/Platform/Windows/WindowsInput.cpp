#include "WindowsInput.h"

#include "Codi/Application.h"

#include "GLFW/glfw3.h"

namespace Codi {

Input* Input::_instance = new WindowsInput();

bool WindowsInput::isKeyPressed(int keycode) {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
    int state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::isMouseButtonPressed(int button) {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
    int state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::getMousePos() {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return { (float)xpos, (float)ypos };
}

float WindowsInput::getMouseX() {
    auto[x, y] = getMousePos();
    return x;
}

float WindowsInput::getMouseY() {
    auto[x, y] = getMousePos();
    return y;
}

}
