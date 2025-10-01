#pragma once

#include "Codi/Events/Event.h"

namespace Codi {

class KeyEvent : public Event {
public:
    inline int getKeyCode() const { return _keyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    KeyEvent(int keycode)
        : _keyCode(keycode) {
    }

    int _keyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), _repeatCount(repeatCount) {}

    inline int GetRepeatCount() const { return _repeatCount; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << _keyCode << " (" << _repeatCount << " repeats)";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)

private:
    int _repeatCount;
};

class KeyReleasedEvent : public KeyEvent {
public:
    KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << _keyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent : public KeyEvent {
public:
    KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << _keyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
};

}
