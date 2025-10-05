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
    
    Ref<Scene> _activeScene;
    Entity _squareEntity;
    Entity _cameraEntity;

    bool _viewportFocused = false;
    bool _viewportHovered = false;
    glm::vec2 _viewportSize = glm::vec2(0.0f);
};

} // namespace Codi
