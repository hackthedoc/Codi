#pragma once

#include "Codi/Core/Core.h"

namespace Codi {

// EVENTS ARE BLOCKING

enum class EventType {
    None = 0,
    WindowClose, WindowResized, WindowFocus, WindowLostFocus, WindowMoved,
    AppTick, AppUpdate, AppRender,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory {
    None = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput = BIT(1),
    EventCategoryKeyboard = BIT(2),
    EventCategoryMouse = BIT(3),
    EventCategoryMouseButton = BIT(4)
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                                virtual EventType getEventType() const override { return GetStaticType(); }\
                                virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

class Event {
friend class EventDispatcher;

public:
    virtual ~Event() = default;

    virtual EventType getEventType() const = 0;
    virtual const char* getName() const = 0;
    virtual int getCategoryFlags() const = 0;
    virtual std::string toString() const { return getName(); }

    inline bool isInCategory(EventCategory category) {
        return getCategoryFlags() & category;
    }

    void handle() { _handled = true; }
    inline bool isHandled() const { return _handled; }

protected:
    bool _handled = false;
};

class EventDispatcher {
public:
    EventDispatcher(Event& event) : _event(event) {}

    template<typename T, typename F>
    bool dispatch(const F& func) {
        if (_event.getEventType() != T::GetStaticType()) return false;
        _event._handled = func(static_cast<T&>(_event));
        return true;
    }

private:
    Event& _event;
};

}
