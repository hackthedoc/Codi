#include "EditorLayer.h"

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Codi {

    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach() {

        _ShroomlightStudioMaterial = MaterialManager::Create("ShroomlightStudioIconMaterial");
        _ShroomlightStudioMaterial->SetAlbedo(TextureManager::Load("Resources/Textures/shroomlight_logo.png"));
        
        // Create Editor Camera

        _EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
    }

    void EditorLayer::OnDetach() {}

    void EditorLayer::OnUpdate(float32 deltatime) {
        _EditorCamera.OnUpdate(deltatime);

        // Rendering

        Renderer2D::ResetStatistics();
        Renderer2D::BeginScene(_EditorCamera);

        static float32 rotationAngle = 0.0f;
        rotationAngle -= glm::radians(90.0f) * deltatime;

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
        glm::mat4 rotation = glm::toMat4(glm::quat(glm::vec3(0.0f, 0.0f, rotationAngle)));
        glm::mat4 position = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
        glm::mat4 transform = position * rotation * scale;

        Renderer2D::DrawQuad(transform, _ShroomlightStudioMaterial);
        //Renderer2D::DrawGeometry(transform, _ShroomlightIcon);

        glm::vec4 purpleColor = glm::vec4(0.741f, 0.576f, 0.976f, 1.0f);
        Renderer2D::DrawQuad(transform, purpleColor);

        Renderer2D::EndScene();
    }

    void EditorLayer::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(CODI_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(CODI_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(const KeyPressedEvent& e) {
        if (e.IsRepeat()) return false;

        const bool ctrlPressed = Input::IsKeyPressed(KeyCode::LeftControl) || Input::IsKeyPressed(KeyCode::RightControl);
        const bool shiftPressed = Input::IsKeyPressed(KeyCode::LeftShift) || Input::IsKeyPressed(KeyCode::RightShift);

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
        _EditorCamera.SetViewportSize((float32)e.GetWidth(), (float32)e.GetHeight());
        return false;
    }

} // namespace Codi
