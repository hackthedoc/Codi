#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/RendererAPI.h"

namespace Codi {

    struct RenderPacket {
        float Deltatime;
    };

    class Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32 width, uint32 height);

        static void DrawFrame(const RenderPacket& packet);

        static RendererAPI& GetRAPI() { return *_RAPI; }

    private:
        static Owned<RendererAPI> _RAPI;
    };

} // namespace Codi
