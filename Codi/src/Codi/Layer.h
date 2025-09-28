#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Codi {

class CAPI Layer{
public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate() {}
    virtual void onImGuiRender() {}
    virtual void onEvent(Event& e) {}

    inline const std::string& getName() const { return _debugName; }

protected:
    std::string _debugName;
};

}
