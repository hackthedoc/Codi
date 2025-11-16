#pragma once

#include "Codi/Core/Types.h"

namespace Codi {

    struct KeyCode {
        enum : int32 {
            UNKNOWN = -1,

            // Alphanumeric
            A = 0, B, C, D, E, F, G, H, I, J,
            K, L, M, N, O, P, Q, R, S, T,
            U, V, W, X, Y, Z,

            D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,

            // Function keys
            F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
            F11, F12, F13, F14, F15, F16, F17, F18, F19, F20,

            // Control / modifiers
            ESCAPE, TAB, CAPS_LOCK, SHIFT, CONTROL, ALT,
            LEFT_SHIFT, RIGHT_SHIFT, LEFT_CONTROL, RIGHT_CONTROL,
            LEFT_ALT, RIGHT_ALT, LEFT_SUPER, RIGHT_SUPER,

            SPACE, ENTER, BACKSPACE,

            // Navigation
            INSERT, DELETE_KEY, HOME, END, PAGE_UP, PAGE_DOWN,
            ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT,

            // Numpad
            NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4,
            NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9,
            NUMPAD_ADD, NUMPAD_SUBTRACT, NUMPAD_MULTIPLY, NUMPAD_DIVIDE, NUMPAD_DECIMAL,

            // Symbols
            SEMICOLON, EQUAL, COMMA, MINUS, PERIOD, SLASH, GRAVE,
            LEFT_BRACKET, BACKSLASH, RIGHT_BRACKET, APOSTROPHE,

            // System keys
            PRINT_SCREEN, SCROLL_LOCK, PAUSE, MENU,
        };
        int32 value;

        constexpr KeyCode(int32 v) : value(v) {}
        constexpr operator int32() const { return value; }
    };

} // namespace Codi
