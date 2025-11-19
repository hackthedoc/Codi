#include "EditorLayer.h"

namespace Codi {

    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {}

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(float deltatime) {
        Renderer2D::StartBatch(); // begin scene

        Renderer2D::DrawQuad();

        Renderer2D::Flush(); // end scene
    }

} // namespace Codi
