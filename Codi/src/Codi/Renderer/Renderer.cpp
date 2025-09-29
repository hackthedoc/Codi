#include "cdpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Codi {

Renderer::SceneData* Renderer::_sceneData = new Renderer::SceneData;

void Renderer::Init() {
    RenderCommand::Init();
}

void Renderer::BeginScene(OrthographicCamera& camera) {
    _sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::EndScene() {}

void Renderer::Submit(const Ref<class Shader>& shader, const Ref<class VertexArray>& vertexArray, const glm::mat4& transform) {
    shader->bind();

    std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_ViewProjection", _sceneData->viewProjectionMatrix);
    std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_Transform", transform);

    vertexArray->bind();
    RenderCommand::DrawIndexed(vertexArray);
}

} // namespace Codi

