#include "cdpch.h"
#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace Codi {

void OpenGLRendererAPI::init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
}

void OpenGLRendererAPI::SetViewport(uint x, uint y, uint width, uint height) {
    glViewport(x, y, width, height);
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const Ref<class VertexArray>& vertexArray, uint32_t indexCount) {
    uint32_t count = indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();
    glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
}

}; // namespace Codi
