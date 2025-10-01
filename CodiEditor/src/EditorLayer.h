#pragma once

#include <Codi.h>

namespace Codi {

class EditorLayer : public Layer {
public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void onAttach() override;
    virtual void onDetach() override;

    void onUpdate(DeltaTime deltatime) override;
    virtual void onImGuiRender() override;
    void onEvent(Event& e) override;

private:
    OrthographicCameraController _cameraController;

    Ref<FrameBuffer> _frameBuffer;
    glm::vec2 _viewportSize;
};

} // namespace Codi
