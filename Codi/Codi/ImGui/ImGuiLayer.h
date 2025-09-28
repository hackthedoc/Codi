#pragma once

#include "Codi/Layer.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

namespace Codi {

class CAPI ImGuiLayer : public Layer {
public:
    ImGuiLayer();
    ~ImGuiLayer();

    void onAttach() override;
    void onDetach() override;
    void onUpdate() override;
private:
    float _time = 0.0f;
};

}
