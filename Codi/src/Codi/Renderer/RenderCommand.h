#pragma once

#include "Codi/Renderer/RendererAPI.h"
#include "glad/glad.h"
namespace Codi {

class RenderCommand {
public:
    inline static void Init() { _rendererAPI->init(); }

    inline static void SetClearColor(const glm::vec4& color) { _rendererAPI->SetClearColor(color); }
    inline static void Clear() { _rendererAPI->Clear(); }

    inline static void DrawIndexed(const Ref<class VertexArray>& vertexArray) {
        _rendererAPI->DrawIndexed(vertexArray);
    }

private:
    static RendererAPI* _rendererAPI;
};

} // namespace Codi
