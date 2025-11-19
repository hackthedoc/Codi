#pragma once

#include "Codi/Core/Core.h"

#include <glm/glm.hpp>

namespace Codi {
    class GraphicsPipeline;
    class VertexBuffer;
    class IndexBuffer;

    struct RenderPacket2D {
        Shared<GraphicsPipeline> Pipeline;
        Shared<VertexBuffer> Vertex;
        Shared<IndexBuffer> Index;

        uint32 VertexCount;
        uint32 IndexCount;
    };

    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        static void StartBatch();
        static void Flush();

        static void DrawQuad(const glm::vec4& color);

    private:
        static void FlushAndReset();
    };

} // namespace Codi
