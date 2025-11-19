#pragma once

#include "Codi/Core/Core.h"

#include <glm/glm.hpp>

namespace Codi {

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
