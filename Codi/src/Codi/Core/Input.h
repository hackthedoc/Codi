#pragma once

#include "Codi/Core/Core.h"

#include "Codi/Core/KeyCodes.h"
#include "Codi/Core/MouseButtonCodes.h"

namespace Codi {

class CAPI Input {
public:
    static bool IsKeyPressed(KeyCode keycode);
    
    static bool IsMouseButtonPressed(MouseCode button);
    static std::pair<float, float> GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
};

}
