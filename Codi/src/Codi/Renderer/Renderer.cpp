#include "cdpch.h"
#include "Renderer.h"

#include "Renderer2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Codi {

Renderer::SceneData* Renderer::_sceneData = new Renderer::SceneData;

void Renderer::Init() {
    CODI_PROFILE_FUNCTION();

    RenderCommand::Init();
    Renderer2D::Init();
}

void Renderer::Shutdown() {
    CODI_PROFILE_FUNCTION();
    
    Renderer2D::Shutdown();
    delete _sceneData;
}

void Renderer::OnWindowResize(uint width, uint height) {
    RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::BeginScene(OrthographicCamera& camera) {
    _sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::EndScene() {}

void Renderer::Submit(const Ref<class Shader>& shader, const Ref<class VertexArray>& vertexArray, const glm::mat4& transform) {
    shader->bind();

    shader->setMat4("u_ViewProjection", _sceneData->viewProjectionMatrix);
    shader->setMat4("u_Transform", transform);

    vertexArray->bind();
    RenderCommand::DrawIndexed(vertexArray);
}

} // namespace Codi

