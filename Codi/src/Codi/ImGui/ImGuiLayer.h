#pragma once

#include "Codi/Core/Layer.h"

#include "Codi/Events/ApplicationEvent.h"
#include "Codi/Events/KeyEvent.h"
#include "Codi/Events/MouseEvent.h"

namespace Codi {

class ImGuiLayer : public Layer {
public:
    ImGuiLayer();
    ~ImGuiLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onEvent(Event& e) override;

    void begin();
    void end();

    void blockEvents(const bool flag);

private:
    bool _blockEvents = false;
    float _time = 0.0f;
};

}
