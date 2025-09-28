#pragma once

#include "Codi/Input.h"

namespace Codi {

class WindowsInput : public Input {
protected:
    bool isKeyPressed(int keycode) override;

    bool isMouseButtonPressed(int button) override;
    std::pair<float, float> getMousePos() override;
    float getMouseX() override;
    float getMouseY() override;
};

}
