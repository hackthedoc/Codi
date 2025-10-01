#include "cdpch.h"
#include "Codi/Core/Input.h"

#include "Codi/Core/Application.h"

#include "GLFW/glfw3.h"

namespace Codi {

bool Input::IsKeyPressed(KeyCode keycode) {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
    int state = glfwGetKey(window, static_cast<uint32_t>(keycode));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(MouseCode button) {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
    int state = glfwGetMouseButton(window, static_cast<uint32_t>(button));
    return state == GLFW_PRESS;
}

std::pair<float, float> Input::GetMousePosition() {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return { (float)xpos, (float)ypos };
}

float Input::GetMouseX() {
    auto[x, y] = GetMousePosition();
    return x;
}

float Input::GetMouseY() {
    auto[x, y] = GetMousePosition();
    return y;
}

}
