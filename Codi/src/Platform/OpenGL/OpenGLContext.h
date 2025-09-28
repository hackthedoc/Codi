#pragma once

#include "Codi/Renderer/GraphicsContext.h"

class GLFWwindow;

namespace Codi {

class OpenGLContext : public GraphicsContext {
public:
    OpenGLContext(GLFWwindow* windowHandle);

    virtual void init() override;
    virtual void swapBuffers() override;

private:
    GLFWwindow* _windowHandle;
};

}
