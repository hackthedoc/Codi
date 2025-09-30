#pragma once

#include "Codi/Core/Core.h"

namespace Codi {

class CAPI Input {
public:
    inline static bool IsKeyPressed(int keycode) { return _instance->isKeyPressed(keycode); }
    
    inline static bool IsMouseButtonPressed(int button) { return _instance->isMouseButtonPressed(button); }
    inline static std::pair<float, float> GetMousePos() { return _instance->getMousePos(); }
    inline static float GetMouseX() { return _instance->getMouseX(); }
    inline static float GetMouseY() { return _instance->getMouseY(); }

protected:
    virtual bool isKeyPressed(int keycode) = 0;

    virtual bool isMouseButtonPressed(int button) = 0;
    virtual std::pair<float, float> getMousePos() = 0;
    virtual float getMouseX() = 0;
    virtual float getMouseY() = 0;
private:
    static Input* _instance;
};

}
