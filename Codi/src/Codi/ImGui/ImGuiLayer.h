#pragma once

#include "Codi/Core/Layer.h"
#include "Codi/Events/Event.h"

namespace Codi {

    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& e) override;

        void Begin();
        void End();
    };

} // namespace Codi
