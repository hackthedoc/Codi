#pragma once

#include "Codi/Renderer/RenderCommand.h"

namespace Codi {

class Renderer {
public:
    static void BeginScene();
    static void EndScene();

    static void Submit(const std::shared_ptr<class VertexArray>& vertexArray);

    inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

} // namespace Codi
