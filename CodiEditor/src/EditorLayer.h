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
    
    Ref<Scene> _scene;
    Entity _squareEntity;
    glm::vec4 _squareColor = { 0.0f, 1.0f, 0.0f, 1.0f };

    bool _viewportFocused = false;
    bool _viewportHovered = false;
    glm::vec2 _viewportSize = glm::vec2(0.0f);
};

} // namespace Codi
