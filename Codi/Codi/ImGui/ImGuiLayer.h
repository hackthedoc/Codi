#pragma once

#include "Codi/Layer.h"

namespace Codi {

class CAPI ImGuiLayer : public Layer {
public:
    ImGuiLayer();
    ~ImGuiLayer();

    void onAttach() override;
    void onDetach() override;
    void onUpdate() override;
    void onEvent(Event& e) override;
private:
    float _time = 0.0f;
};

}
