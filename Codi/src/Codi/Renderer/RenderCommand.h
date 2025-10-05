#pragma once

#include "Codi/Renderer/RendererAPI.h"
#include "glad/glad.h"
namespace Codi {

class RenderCommand {
public:
    inline static void Init() { _rendererAPI->init(); }

    inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { _rendererAPI->SetViewport(x, y, width, height); }

    inline static void SetClearColor(const glm::vec4& color) { _rendererAPI->SetClearColor(color); }
    inline static void Clear() { _rendererAPI->Clear(); }

    inline static void DrawIndexed(const Ref<class VertexArray>& vertexArray, uint32_t indexCount = 0) {
        _rendererAPI->DrawIndexed(vertexArray, indexCount);
    }

private:
    static RendererAPI* _rendererAPI;
};

} // namespace Codi
