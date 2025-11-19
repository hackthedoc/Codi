#include "EditorLayer.h"

#include <imgui/imgui.h>

namespace Codi {

    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {}

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(float32 deltatime) {
        Renderer2D::StartBatch(); // begin scene

        glm::vec4 purpleColor = glm::vec4(0.741f, 0.576f, 0.976f, 1.0f);
        Renderer2D::DrawQuad(purpleColor);

        Renderer2D::Flush(); // end scene
    }

    void EditorLayer::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(CODI_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(CODI_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(const KeyPressedEvent& e) {
        if (e.IsRepeat()) return false;

        const bool ctrlPressed = Input::IsKeyPressed(KeyCode::LEFT_CONTROL) || Input::IsKeyPressed(KeyCode::RIGHT_CONTROL);
        const bool shiftPressed = Input::IsKeyPressed(KeyCode::LEFT_SHIFT) || Input::IsKeyPressed(KeyCode::RIGHT_SHIFT);

        switch (e.GetKeyCode()) {
        case KeyCode::Q:
        {
            if (ctrlPressed && shiftPressed) Application::Get().Close();
            break;
        }
        }

        return false;
    }

    bool EditorLayer::OnMouseButtonPressed(const MouseButtonPressedEvent& e) {
        return false;
    }

} // namespace Codi
