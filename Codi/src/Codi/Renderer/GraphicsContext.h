#pragma once

namespace Codi {

class GraphicsContext {
public:
    virtual void init() = 0;
    virtual void swapBuffers() = 0;
};

}