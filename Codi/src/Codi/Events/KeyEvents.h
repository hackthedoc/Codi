#pragma once

#include "Codi/Events/Event.h"

// TODO: Refactor to use Codi::string
#include <sstream>

namespace Codi {

    class KeyEvent : public Event {
    public:
        inline int32 GetKeyCode() const { return _KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        KeyEvent(int32 keycode) : _KeyCode(keycode) {}
        int32 _KeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int32 keycode, bool isRepeat = false) : KeyEvent(keycode), _IsRepeat(isRepeat) {}

        inline bool IsRepeat() const { return _IsRepeat; }

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << _KeyCode << " (repeats = " << _IsRepeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        bool _IsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int32 keycode) : KeyEvent(keycode) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << _KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent {
    public:
        KeyTypedEvent(int32 keycode) : KeyEvent(keycode) {}

        std::string ToString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << _KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };

} // namespace Codi
