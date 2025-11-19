#include "EditorLayer.h"

namespace Codi {

    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {}

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(float deltatime) {
        Renderer2D::StartBatch(); // begin scene

        glm::vec4 purpleColor = glm::vec4(0.741f, 0.576f, 0.976f, 1.0f);
        Renderer2D::DrawQuad(purpleColor);

        Renderer2D::Flush(); // end scene
    }

} // namespace Codi
