#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D")
    , _cameraController(1280.0f / 720.0f, true)
    {}

void Sandbox2D::onAttach() {
    CODI_PROFILE_FUNCTION();
    
    _texture = Codi::Texture2D::Create("assets/textures/love.png");
}

void Sandbox2D::onDetach() {
    CODI_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Codi::DeltaTime deltatime) {
    CODI_PROFILE_FUNCTION();
    
    //////// UPDATING //////////
    
    if (Codi::Input::IsKeyPressed(CODI_KEY_ESCAPE))
        Codi::Application::Get().close();
    
    {
        CODI_PROFILE_SCOPE("CameraController::onUpdate");
        _cameraController.onUpdate(deltatime);
    }

    ////////// RENDERING //////////

    {
        CODI_PROFILE_SCOPE("Renderer Clear");
        Codi::RenderCommand::SetClearColor({ 0.0863f, 0.0902f, 0.1137f, 1.0f });
        Codi::RenderCommand::Clear();
    }

    {
        CODI_PROFILE_SCOPE("Renderer2D DrawScene");
        Codi::Renderer2D::BeginScene(_cameraController.getCamera());

        Codi::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f, -0.1f }, { 2.0f, 2.0f }, glm::radians(45.0f), _squareColor);
        Codi::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, _texture);

        Codi::Renderer2D::EndScene();
    }
}

void Sandbox2D::onImGuiRender() {
    CODI_PROFILE_FUNCTION();
    
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(_squareColor));

    ImGui::End();
}

void Sandbox2D::onEvent(Codi::Event& e) {
    _cameraController.onEvent(e);
}
