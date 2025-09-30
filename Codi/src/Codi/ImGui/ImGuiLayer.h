#pragma once

#include "Codi/Core/Layer.h"

#include "Codi/Events/ApplicationEvent.h"
#include "Codi/Events/KeyEvent.h"
#include "Codi/Events/MouseEvent.h"

namespace Codi {

class CAPI ImGuiLayer : public Layer {
public:
    ImGuiLayer();
    ~ImGuiLayer();

    void onAttach() override;
    void onDetach() override;
    void onImGuiRender() override;

    void begin();
    void end();
private:
    float _time = 0.0f;
};

}
