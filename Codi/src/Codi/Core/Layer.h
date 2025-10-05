#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Core/DeltaTime.h"
#include "Codi/Events/Event.h"

namespace Codi {

class Layer{
public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate(DeltaTime ts) {}
    virtual void onImGuiRender() {}
    virtual void onEvent(Event& e) {}

    inline const std::string& getName() const { return _debugName; }

protected:
    std::string _debugName;
};

}
