#include "codipch.h"
#include "Codi/Input/Input.h"

#include "Platform/Windows/WindowsInputUtils.h"

#include <SDL3/SDL.h>

namespace Codi {

    bool Input::IsKeyPressed(KeyCode kcode) {
        SDL_Keycode skcode = Utils::KeyCodeToSDL(kcode);
        const bool* state = SDL_GetKeyboardState(nullptr);
        SDL_Scancode scode = SDL_GetScancodeFromKey(skcode, nullptr);
        return state[scode];
    }

    bool Input::IsMouseButtonPressed(MouseCode mcode) {
        uint8 smcode = Utils::MouseCodeToSDL(mcode);
        SDL_MouseButtonFlags buttons = SDL_GetMouseState(nullptr, nullptr);
        return (buttons & SDL_BUTTON_MASK(smcode)) != 0;
    }

    std::pair<float32, float32> Input::GetMousePosition() {
        float32 x = 0;
        float32 y = 0;
        SDL_GetMouseState(&x, &y);
        return { x, y };
    }

    float32 Input::GetMouseX() {
        float32 x = 0;
        SDL_GetMouseState(&x, nullptr);
        return x;
    }

    float32 Input::GetMouseY() {
        float32 y = 0;
        SDL_GetMouseState(nullptr, &y);
        return y;
    }


} // namespace Codi
