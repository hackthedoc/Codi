#pragma once

#include "Codi/Renderer/RendererAPI.h"

namespace Codi {

class OpenGLRendererAPI : public RendererAPI {
public:
    virtual void init() override;

    virtual void SetViewport(uint x, uint y, uint width, uint height) override;

    virtual void SetClearColor(const glm::vec4& color) override;
    virtual void Clear() override;

    virtual void DrawIndexed(const Ref<class VertexArray>& vertexArray, uint32_t indexCount = 0) override;
};

} // namespace Codi
