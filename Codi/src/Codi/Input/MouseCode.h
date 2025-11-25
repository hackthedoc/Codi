#pragma once

#include "Codi/Core/Types.h"

namespace Codi {

    struct MouseCode {
        enum : int32 {
            ButtonLeft = 0,
            ButtonRight = 1,
            ButtonMiddle = 2,
            ButtonX1 = 3,  // Typically "Back" button
            ButtonX2 = 4,  // Typically "Forward" button

            ScrollUp = 5,
            ScrollDown = 6,
            ScrollLeft = 7,
            ScrollRight = 8,
        };
        int32 value;

        constexpr MouseCode(int32 v) : value(v) {}
        constexpr operator int32() const { return value; }
    };

} // namespace Codi
