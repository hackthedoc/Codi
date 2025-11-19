#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Events/Event.h"

namespace Codi {

    class Layer {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float32 deltatime) {}
        virtual void OnImGuiRender() {}
        virtual void OnEvent(Event& e) {}

        inline const std::string& GetName() const { return _DebugName; }

    protected:
        std::string _DebugName;
    };

} // namespace Codi
