#pragma once

#include "Codi/Core/Log.h"
#include "Codi/Input/KeyCode.h"
#include "Codi/Input/MouseCode.h"

#include <SDL3/SDL.h>

namespace Codi::Utils {

    // -------------------------
    // Key Mapping
    // -------------------------

    inline static SDL_Keycode KeyCodeToSDL(KeyCode code) {
        switch (code) {
        case KeyCode::A: return SDLK_A;
        case KeyCode::B: return SDLK_B;
        case KeyCode::C: return SDLK_C;
        case KeyCode::D: return SDLK_D;
        case KeyCode::E: return SDLK_E;
        case KeyCode::F: return SDLK_F;
        case KeyCode::G: return SDLK_G;
        case KeyCode::H: return SDLK_H;
        case KeyCode::I: return SDLK_I;
        case KeyCode::J: return SDLK_J;
        case KeyCode::K: return SDLK_K;
        case KeyCode::L: return SDLK_L;
        case KeyCode::M: return SDLK_M;
        case KeyCode::N: return SDLK_N;
        case KeyCode::O: return SDLK_O;
        case KeyCode::P: return SDLK_P;
        case KeyCode::Q: return SDLK_Q;
        case KeyCode::R: return SDLK_R;
        case KeyCode::S: return SDLK_S;
        case KeyCode::T: return SDLK_T;
        case KeyCode::U: return SDLK_U;
        case KeyCode::V: return SDLK_V;
        case KeyCode::W: return SDLK_W;
        case KeyCode::X: return SDLK_X;
        case KeyCode::Y: return SDLK_Y;
        case KeyCode::Z: return SDLK_Z;

        case KeyCode::D0: return SDLK_0;
        case KeyCode::D1: return SDLK_1;
        case KeyCode::D2: return SDLK_2;
        case KeyCode::D3: return SDLK_3;
        case KeyCode::D4: return SDLK_4;
        case KeyCode::D5: return SDLK_5;
        case KeyCode::D6: return SDLK_6;
        case KeyCode::D7: return SDLK_7;
        case KeyCode::D8: return SDLK_8;
        case KeyCode::D9: return SDLK_9;

        case KeyCode::SPACE: return SDLK_SPACE;
        case KeyCode::ENTER: return SDLK_RETURN;
        case KeyCode::BACKSPACE: return SDLK_BACKSPACE;
        case KeyCode::TAB: return SDLK_TAB;
        case KeyCode::ESCAPE: return SDLK_ESCAPE;

        case KeyCode::LEFT_SHIFT: return SDLK_LSHIFT;
        case KeyCode::RIGHT_SHIFT: return SDLK_RSHIFT;
        case KeyCode::LEFT_CONTROL: return SDLK_LCTRL;
        case KeyCode::RIGHT_CONTROL: return SDLK_RCTRL;
        case KeyCode::LEFT_ALT: return SDLK_LALT;
        case KeyCode::RIGHT_ALT: return SDLK_RALT;

        case KeyCode::ARROW_UP: return SDLK_UP;
        case KeyCode::ARROW_DOWN: return SDLK_DOWN;
        case KeyCode::ARROW_LEFT: return SDLK_LEFT;
        case KeyCode::ARROW_RIGHT: return SDLK_RIGHT;

        case KeyCode::INSERT: return SDLK_INSERT;
        case KeyCode::DELETE_KEY: return SDLK_DELETE;
        case KeyCode::HOME: return SDLK_HOME;
        case KeyCode::END: return SDLK_END;
        case KeyCode::PAGE_UP: return SDLK_PAGEUP;
        case KeyCode::PAGE_DOWN: return SDLK_PAGEDOWN;

        case KeyCode::F1: return SDLK_F1;
        case KeyCode::F2: return SDLK_F2;
        case KeyCode::F3: return SDLK_F3;
        case KeyCode::F4: return SDLK_F4;
        case KeyCode::F5: return SDLK_F5;
        case KeyCode::F6: return SDLK_F6;
        case KeyCode::F7: return SDLK_F7;
        case KeyCode::F8: return SDLK_F8;
        case KeyCode::F9: return SDLK_F9;
        case KeyCode::F10: return SDLK_F10;
        case KeyCode::F11: return SDLK_F11;
        case KeyCode::F12: return SDLK_F12;

        default: return SDLK_UNKNOWN;
        }
    }

    inline static KeyCode KeyCodeFromSDL(SDL_Keycode code) {
        switch (code) {
        case SDLK_A: return KeyCode::A;
        case SDLK_B: return KeyCode::B;
        case SDLK_C: return KeyCode::C;
        case SDLK_D: return KeyCode::D;
        case SDLK_E: return KeyCode::E;
        case SDLK_F: return KeyCode::F;
        case SDLK_G: return KeyCode::G;
        case SDLK_H: return KeyCode::H;
        case SDLK_I: return KeyCode::I;
        case SDLK_J: return KeyCode::J;
        case SDLK_K: return KeyCode::K;
        case SDLK_L: return KeyCode::L;
        case SDLK_M: return KeyCode::M;
        case SDLK_N: return KeyCode::N;
        case SDLK_O: return KeyCode::O;
        case SDLK_P: return KeyCode::P;
        case SDLK_Q: return KeyCode::Q;
        case SDLK_R: return KeyCode::R;
        case SDLK_S: return KeyCode::S;
        case SDLK_T: return KeyCode::T;
        case SDLK_U: return KeyCode::U;
        case SDLK_V: return KeyCode::V;
        case SDLK_W: return KeyCode::W;
        case SDLK_X: return KeyCode::X;
        case SDLK_Y: return KeyCode::Y;
        case SDLK_Z: return KeyCode::Z;

        case SDLK_0: return KeyCode::D0;
        case SDLK_1: return KeyCode::D1;
        case SDLK_2: return KeyCode::D2;
        case SDLK_3: return KeyCode::D3;
        case SDLK_4: return KeyCode::D4;
        case SDLK_5: return KeyCode::D5;
        case SDLK_6: return KeyCode::D6;
        case SDLK_7: return KeyCode::D7;
        case SDLK_8: return KeyCode::D8;
        case SDLK_9: return KeyCode::D9;

        case SDLK_SPACE: return KeyCode::SPACE;
        case SDLK_RETURN: return KeyCode::ENTER;
        case SDLK_BACKSPACE: return KeyCode::BACKSPACE;
        case SDLK_TAB: return KeyCode::TAB;
        case SDLK_ESCAPE: return KeyCode::ESCAPE;

        case SDLK_LSHIFT: return KeyCode::LEFT_SHIFT;
        case SDLK_RSHIFT: return KeyCode::RIGHT_SHIFT;
        case SDLK_LCTRL: return KeyCode::LEFT_CONTROL;
        case SDLK_RCTRL: return KeyCode::RIGHT_CONTROL;
        case SDLK_LALT: return KeyCode::LEFT_ALT;
        case SDLK_RALT: return KeyCode::RIGHT_ALT;

        case SDLK_UP: return KeyCode::ARROW_UP;
        case SDLK_DOWN: return KeyCode::ARROW_DOWN;
        case SDLK_LEFT: return KeyCode::ARROW_LEFT;
        case SDLK_RIGHT: return KeyCode::ARROW_RIGHT;

        case SDLK_INSERT: return KeyCode::INSERT;
        case SDLK_DELETE: return KeyCode::DELETE_KEY;
        case SDLK_HOME: return KeyCode::HOME;
        case SDLK_END: return KeyCode::END;
        case SDLK_PAGEUP: return KeyCode::PAGE_UP;
        case SDLK_PAGEDOWN: return KeyCode::PAGE_DOWN;

        case SDLK_F1: return KeyCode::F1;
        case SDLK_F2: return KeyCode::F2;
        case SDLK_F3: return KeyCode::F3;
        case SDLK_F4: return KeyCode::F4;
        case SDLK_F5: return KeyCode::F5;
        case SDLK_F6: return KeyCode::F6;
        case SDLK_F7: return KeyCode::F7;
        case SDLK_F8: return KeyCode::F8;
        case SDLK_F9: return KeyCode::F9;
        case SDLK_F10: return KeyCode::F10;
        case SDLK_F11: return KeyCode::F11;
        case SDLK_F12: return KeyCode::F12;

        default:
            CODI_CORE_WARN("Unknown SDL keycode: {0}", (int32)code);
            return KeyCode::UNKNOWN;
        }
    }

    // -------------------------
    // Mouse Mapping
    // -------------------------

    inline static uint8 MouseCodeToSDL(MouseCode code) {
        switch (code) {
        case MouseCode::BUTTON_LEFT: return SDL_BUTTON_LEFT;
        case MouseCode::BUTTON_RIGHT: return SDL_BUTTON_RIGHT;
        case MouseCode::BUTTON_MIDDLE: return SDL_BUTTON_MIDDLE;
        case MouseCode::BUTTON_X1: return SDL_BUTTON_X1;
        case MouseCode::BUTTON_X2: return SDL_BUTTON_X2;
        default: return SDL_BUTTON_LEFT;
        }
    }

    inline static MouseCode MouseCodeFromSDL(uint8 code) {
        switch (code) {
        case SDL_BUTTON_LEFT: return MouseCode::BUTTON_LEFT;
        case SDL_BUTTON_RIGHT: return MouseCode::BUTTON_RIGHT;
        case SDL_BUTTON_MIDDLE: return MouseCode::BUTTON_MIDDLE;
        case SDL_BUTTON_X1: return MouseCode::BUTTON_X1;
        case SDL_BUTTON_X2: return MouseCode::BUTTON_X2;
        default:
            CODI_CORE_WARN("Unknown SDL mouse code: {0}", code);
            return MouseCode::BUTTON_LEFT;
        }
    }

} // namespace Codi::Utils
