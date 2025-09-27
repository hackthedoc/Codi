#pragma once

#include "Event.h"

namespace Codi {

class CAPI MouseMovedEvent : public Event {
public:
    MouseMovedEvent(float x, float y) : _x(x), _y(y) {}
    
    inline float getX() const { return _x; }
    inline float getY() const { return _y; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseMovedEvent: " << _x << ", " << _y;
        return ss.str();
    }
    
    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float _x;
    float _y;
};

class CAPI MouseScrolledEvent : public Event {
public:
    MouseScrolledEvent(float xOffset, float yOffset)
        : _xOffset(xOffset), _yOffset(yOffset) {
    }
    
    inline float getXOffset() const { return _xOffset; }
    inline float getYOffset() const { return _yOffset; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
        return ss.str();
    }
    
    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float _xOffset;
    float _yOffset;
};

class CAPI MouseButtonEvent : public Event {
public:
    inline int GetMouseButton() const { return _button; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

protected:
    MouseButtonEvent(int button)
        : _button(button) {
    }

    int _button;
};

class CAPI MouseButtonPressedEvent : public MouseButtonEvent {
public:
    MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseButtonPressedEvent: " << _button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class CAPI MouseButtonReleasedEvent : public MouseButtonEvent {
public:
    MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseButtonReleasedEvent: " << _button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

}