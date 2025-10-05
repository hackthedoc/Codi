#pragma once

#include "Codi/Renderer/RendererAPI.h"

namespace Codi {

class OpenGLRendererAPI : public RendererAPI {
public:
    virtual void init() override;

    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    virtual void SetClearColor(const glm::vec4& color) override;
    virtual void Clear() override;

    virtual void DrawIndexed(const Ref<class VertexArray>& vertexArray, uint32_t indexCount = 0) override;
};

} // namespace Codi
