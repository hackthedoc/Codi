#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/RendererAPI.h"

namespace Codi {

    class Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32 width, uint32 height);

        static void BeginFrame();
        static void EndFrame();

        static void DrawIndexed(const Shared<VertexArray>& vertexArray, uint32 indexCount);

        static inline bool IsFrameSkipped() { return _FrameSkipped; }

        static RendererAPI& GetRAPI() { return *_RAPI; }

    private:
        static Owned<RendererAPI> _RAPI;
        static bool _FrameSkipped;
    };

} // namespace Codi
