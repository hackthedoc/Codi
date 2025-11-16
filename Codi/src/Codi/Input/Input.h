#pragma once

#include "Codi/Core/Core.h"

#include "Codi/Input/KeyCode.h"
#include "Codi/Input/MouseCode.h"

namespace Codi {

    class Input {
    public:
        static bool IsKeyPressed(KeyCode kcode);

        static bool IsMouseButtonPressed(MouseCode mcode);
        static std::pair<float32, float32> GetMousePosition();
        static float32 GetMouseX();
        static float32 GetMouseY();
    };

} // namespace Codi
