#include "cdpch.h"
#include "OpenGLContext.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Codi {
   
OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : _windowHandle(windowHandle) {
    CODI_CORE_ASSERT(windowHandle, "Window handle is null!");
}

void OpenGLContext::init() {
    glfwMakeContextCurrent(_windowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    CODI_CORE_ASSERT(status, "Failed to initialize GLAD!");
}

void OpenGLContext::swapBuffers() {
    glfwSwapBuffers(_windowHandle);
}

}