#pragma once

#include "Codi/Core/Core.h"

#include "Codi/Renderer/EditorCamera.h"

#include <glm/glm.hpp>

namespace Codi {

    class Renderer2D {
    public:
        struct Statistics {
            uint32 DrawCalls = 0;
            uint32 QuadCount = 0;

            uint32 GetTotalVertexCount() const { return QuadCount * 4; }
            uint32 GetTotalIndexCount() const { return QuadCount * 6; }

            void Reset() { DrawCalls = 0; QuadCount = 0; }
        };

    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const EditorCamera& camera);
        static void EndScene();
        static void StartBatch();
        static void Flush();

        // Primitives

        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);

        // Internals

        static void ResetStatistics();
        static Statistics GetStatistics();

    private:
        static void FlushAndReset();
    };

} // namespace Codi
