#include "EditorLayer.h"

namespace Codi {

    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {}

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(float deltatime) {
        Renderer2D::StartBatch(); // begin scene

        glm::vec4 clearColor = glm::vec4(0.0863f, 0.0902f, 0.1137f, 1.0f);
        Renderer2D::DrawQuad(clearColor);

        Renderer2D::Flush(); // end scene
    }

} // namespace Codi
