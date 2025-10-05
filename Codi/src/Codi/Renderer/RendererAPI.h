#pragma once

#include "glm/glm.hpp"

#include "Codi/Renderer/VertexArray.h"

namespace Codi {
    
class RendererAPI {
public:
    enum class API {
        None = 0,
        OpenGL = 1
    };

    virtual ~RendererAPI() = default;

    virtual void init() = 0;

    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    virtual void SetClearColor(const glm::vec4& color) = 0;
    virtual void Clear() = 0;

    virtual void DrawIndexed(const Ref<class VertexArray>& vertexArray, uint32_t indexCount = 0)= 0;

    inline static API GetAPI() { return _api; }

private:
    static API _api;
};

} // namespace Codi

