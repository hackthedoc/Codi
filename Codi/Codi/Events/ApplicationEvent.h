#pragma once

#include "Event.h"

namespace Codi {

class CAPI WindowResizeEvent : public Event {
public:
    WindowResizeEvent(uint width, uint height) : _width(width), _height(height) {}

    inline uint getWidth() const { return _width; }
    inline uint getHeight() const { return _height; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << _width << ", " << _height;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowResized)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    uint _width;
    uint _height;
};

class CAPI WindowCloseEvent : public Event {
    WindowCloseEvent() {}


    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class CAPI AppTickEvent : public Event {
    AppTickEvent() {}


    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class CAPI AppUpdateEvent : public Event {
    AppUpdateEvent() {}


    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class CAPI AppRenderEvent : public Event {
    AppRenderEvent() {}


    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

}
