#include "cdpch.h"
#include "Renderer.h"

namespace Codi {

void Renderer::BeginScene() {}

void Renderer::EndScene() {}

void Renderer::Submit(const std::shared_ptr<class VertexArray>& vertexArray) {
    RenderCommand::DrawIndexed(vertexArray);
}

} // namespace Codi

