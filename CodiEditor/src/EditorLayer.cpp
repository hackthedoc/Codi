#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Codi {

EditorLayer::EditorLayer()
    : Layer("EditorLayer")
    , _cameraController(1280.0f / 720.0f)
    {}

void EditorLayer::onAttach() {
    CODI_PROFILE_FUNCTION();
    
    FrameBufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    _frameBuffer = FrameBuffer::Create(fbSpec);

   _scene = CreateRef<Scene>();
   _squareEntity = _scene->createEntity("Square");
   _squareEntity.addComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
}

void EditorLayer::onDetach() {
    CODI_PROFILE_FUNCTION();
}

void EditorLayer::onUpdate(DeltaTime deltatime) {
    CODI_PROFILE_FUNCTION();
    
    //////// UPDATING //////////
    
    if (Input::IsKeyPressed(KeyCode::KEY_ESCAPE))
        Application::Get().close();
    
    if (_viewportFocused) {
        CODI_PROFILE_SCOPE("CameraController::onUpdate");
        _cameraController.onUpdate(deltatime);
    }

    ////////// RENDERING //////////

    Renderer2D::ResetStats();

    _frameBuffer->bind();
    RenderCommand::SetClearColor({ 0.0863f, 0.0902f, 0.1137f, 1.0f });
    RenderCommand::Clear();


    Renderer2D::BeginScene(_cameraController.getCamera());
    _scene->onUpdate(deltatime);
    Renderer2D::EndScene();

    _frameBuffer->unbind();
}

void EditorLayer::onImGuiRender() {
    CODI_PROFILE_FUNCTION();
    
    static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }
    
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("Codi Engine", &dockspaceOpen, window_flags);

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
    
    if (!opt_padding)
        ImGui::PopStyleVar();
    
    if (opt_fullscreen)
        ImGui::PopStyleVar(2);
    
    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    
    }
    
    ImGui::Begin("Settings");
    ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    auto stats = Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    if (_squareEntity) {
        SpriteRendererComponent& squareRenderer = _squareEntity.getComponent<SpriteRendererComponent>();
        ImGui::ColorEdit4("Square Color", glm::value_ptr(squareRenderer.color));
    }
    ImGui::End();

    ImGui::PushStyleVar (ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");
    _viewportFocused = ImGui::IsWindowFocused();
    _viewportHovered = ImGui::IsWindowHovered();
    Application::Get().getImGuiLayer()->blockEvents(!_viewportFocused || !_viewportHovered);
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if (_viewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x && viewportPanelSize.y) {
        _viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        _frameBuffer->resize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
        //_cameraController.onResize(_viewportSize.x, _viewportSize.y);
    }
    uint32_t textureID = _frameBuffer->getColorAttachmentRendererID();
    ImGui::Image((void*)textureID, viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End(); // Main DockSpace
}

void EditorLayer::onEvent(Event& e) {
    _cameraController.onEvent(e);
}

}// namespace Codi
