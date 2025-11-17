#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/RendererAPI.h"

namespace Codi {

    class Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void DrawFrame();

    private:
        static Owned<RendererAPI> _RAPI;
    };

} // namespace Codi
