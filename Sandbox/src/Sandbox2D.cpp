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

    Codi::FrameBufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    _frameBuffer = Codi::FrameBuffer::Create(fbSpec);
}

void Sandbox2D::onDetach() {
    CODI_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Codi::DeltaTime deltatime) {
    CODI_PROFILE_FUNCTION();
    
    //////// UPDATING //////////
    
    if (Codi::Input::IsKeyPressed(Codi::KeyCode::ESCAPE))
        Codi::Application::Get().close();
    
    {
        CODI_PROFILE_SCOPE("CameraController::onUpdate");
        _cameraController.onUpdate(deltatime);
    }

    ////////// RENDERING //////////

    Codi::Renderer2D::ResetStats();

    {
        CODI_PROFILE_SCOPE("Renderer Clear");
        _frameBuffer->bind();
        Codi::RenderCommand::SetClearColor({ 0.0863f, 0.0902f, 0.1137f, 1.0f });
        Codi::RenderCommand::Clear();
    }

    { 
        CODI_PROFILE_SCOPE("Renderer2D DrawScene");
        Codi::Renderer2D::BeginScene(_cameraController.getCamera());

        for (float y = -5.0f; y < 5.0f; y += 0.25f)
            for (float x = -5.0f; x < 5.0f; x += 0.25f)
                Codi::Renderer2D::DrawQuad({ x, y }, { 0.24f, 0.24f }, { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.75f });

        Codi::Renderer2D::EndScene();
        _frameBuffer->unbind();
    }
}

void Sandbox2D::onImGuiRender() {
    CODI_PROFILE_FUNCTION();
    
    ImGui::Begin("Stats");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    auto stats = Codi::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();
}

void Sandbox2D::onEvent(Codi::Event& e) {
    _cameraController.onEvent(e);
}
