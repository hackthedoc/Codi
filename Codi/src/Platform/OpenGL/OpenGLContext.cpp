#include "cdpch.h"
#include "OpenGLContext.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Codi {
   
OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : _windowHandle(windowHandle) {
    CODI_CORE_ASSERT(windowHandle, "Window handle is null!");
}

void OpenGLContext::init() {
    CODI_PROFILE_FUNCTION();

    glfwMakeContextCurrent(_windowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    CODI_CORE_ASSERT(status, "Failed to initialize GLAD!");
    
    // LOGGING OPENGL INFO
    const char* vendor  = reinterpret_cast<const char*>(glGetString(GL_VENDOR));   // get vendor string
    const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER)); // get renderer string
    const char* version  = reinterpret_cast<const char*>(glGetString(GL_VERSION));  // version as string
    CODI_CORE_INFO("OpenGL Info:");
    CODI_CORE_INFO("  Vendor:   {0}", vendor);
    CODI_CORE_INFO("  Renderer: {0}", renderer);
    CODI_CORE_INFO("  Version:  {0}", version);
}

void OpenGLContext::swapBuffers() {
    CODI_PROFILE_FUNCTION();

    glfwSwapBuffers(_windowHandle);
}

}