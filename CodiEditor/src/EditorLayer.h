#pragma once

#include <Codi.h>

#include "Panels/SceneHierarchyPanel.h"

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
    bool onKeyPressed(const KeyPressedEvent& e);

private:
    void _createNewScene();
    void _openScene();
    void _saveSceneAs();

    Ref<FrameBuffer> _frameBuffer;
    
    EditorCamera _editorCamera;
    Ref<Scene> _activeScene;

    bool _viewportFocused = false;
    bool _viewportHovered = false;
    glm::vec2 _viewportSize = glm::vec2(0.0f);

    int _guizmoType = -1;

    SceneHierarchyPanel _hierarchyPanel;
};

} // namespace Codi
