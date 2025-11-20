#include "EditorLayer.h"

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Codi {

    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {
        // Create Editor Camera

        _EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
    }

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(float32 deltatime) {
        _EditorCamera.OnUpdate(deltatime);

        // Rendering

        Renderer2D::ResetStatistics();
        Renderer2D::BeginScene(_EditorCamera); // begin scene

        // Position at (0,0,0), default scale (1,1,1), no rotation
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f); // radians
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

        // Build transform
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1))
            * glm::scale(glm::mat4(1.0f), scale);

        glm::vec4 purpleColor = glm::vec4(0.741f, 0.576f, 0.976f, 1.0f);
        Renderer2D::DrawQuad(transform, purpleColor);

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

    bool EditorLayer::OnWindowResize(const WindowResizeEvent& e) {
        _EditorCamera.SetViewportSize(e.GetWidth(), e.GetHeight());
        return false;
    }

} // namespace Codi
