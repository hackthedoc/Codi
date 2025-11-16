#pragma once

#include "Codi/Core/Types.h"

namespace Codi {

    struct MouseCode {
        enum : int32 {
            BUTTON_LEFT = 0,
            BUTTON_RIGHT = 1,
            BUTTON_MIDDLE = 2,
            BUTTON_X1 = 3,  // Typically "Back" button on some mice
            BUTTON_X2 = 4,  // Typically "Forward" button

            SCROLL_UP = 5,
            SCROLL_DOWN = 6,
            SCROLL_LEFT = 7,
            SCROLL_RIGHT = 8,
        };
        int32 value;

        constexpr MouseCode(int32 v) : value(v) {}
        constexpr operator int32() const { return value; }
    };

} // namespace Codi
