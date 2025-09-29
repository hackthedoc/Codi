#include "cdpch.h"
#include "Renderer.h"

namespace Codi {

Renderer::SceneData* Renderer::_sceneData = new Renderer::SceneData;

void Renderer::BeginScene(OrthographicCamera& camera) {
    _sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::EndScene() {}

void Renderer::Submit(const std::shared_ptr<class Shader>& shader, const std::shared_ptr<class VertexArray>& vertexArray, const glm::mat4& transform) {
    shader->bind();
    shader->uploadUniformMat4("u_ViewProjection", _sceneData->viewProjectionMatrix);
    shader->uploadUniformMat4("u_Transform", transform);

    vertexArray->bind();
    RenderCommand::DrawIndexed(vertexArray);
}

} // namespace Codi

