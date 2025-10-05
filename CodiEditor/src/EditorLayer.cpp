#include "EditorLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>

namespace Codi {

EditorLayer::EditorLayer()
    : Layer("EditorLayer")
    {}

void EditorLayer::onAttach() {
    CODI_PROFILE_FUNCTION();

    FrameBufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    _frameBuffer = FrameBuffer::Create(fbSpec);

    _activeScene = CreateRef<Scene>();

    Entity cameraEntity = _activeScene->createEntity("Camera Entity");
    cameraEntity.addComponent<CameraComponent>();

    class CameraController : public ScriptableEntity {
    public:
        virtual void onUpdate(DeltaTime deltatime) override {
            glm::vec3& translation = getComponent<TransformComponent>().translation;
            const float speed = 5.0f;

            if (Input::IsKeyPressed(KeyCode::W)) translation[1] -= speed * deltatime;
            if (Input::IsKeyPressed(KeyCode::A)) translation[0] += speed * deltatime;
            if (Input::IsKeyPressed(KeyCode::S)) translation[1] += speed * deltatime;
            if (Input::IsKeyPressed(KeyCode::D)) translation[0] -= speed * deltatime;
        }
   };

   cameraEntity.addComponent<NativeScriptComponent>().bind<CameraController>();

   _hierarchyPanel.setContext(_activeScene);
}

void EditorLayer::onDetach() {
    CODI_PROFILE_FUNCTION();
}

void EditorLayer::onUpdate(DeltaTime deltatime) {
    CODI_PROFILE_FUNCTION();
    
    if (Input::IsKeyPressed(KeyCode::ESCAPE))
        Application::Get().close();
    
    if (FrameBufferSpecification spec = _frameBuffer->getSpecification();
        _viewportSize.x > 0.0f && _viewportSize.y > 0 &&
        (spec.width != _viewportSize.x || spec.height != _viewportSize.y)
        ) {
        _frameBuffer->resize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);

        _activeScene->onViewportResize((uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
    }

    Renderer2D::ResetStats();

    _frameBuffer->bind();
    RenderCommand::SetViewport(0, 0, (uint32_t)_viewportSize.x, (uint32_t)_viewportSize.y);
    RenderCommand::SetClearColor({ 0.0863f, 0.0902f, 0.1137f, 1.0f });
    RenderCommand::Clear();

    _activeScene->onUpdate(deltatime);
    
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

    _hierarchyPanel.onImGuiRender();
    
    ImGui::Begin("Stats");
    ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    auto stats = Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();

    ImGui::PushStyleVar (ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");
    
    _viewportFocused = ImGui::IsWindowFocused();
    _viewportHovered = ImGui::IsWindowHovered();
    
    Application::Get().getImGuiLayer()->blockEvents(!_viewportFocused || !_viewportHovered);
    
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    _viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
    
    uint32_t textureID = _frameBuffer->getColorAttachmentRendererID();
    ImGui::Image((void*)textureID, ImVec2{ _viewportSize.x, _viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End(); // Main DockSpace
}

void EditorLayer::onEvent(Event& e) {}

}// namespace Codi
