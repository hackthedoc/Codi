#pragma once

#include "Codi/Core/Core.h"

#include "Codi/Renderer/EditorCamera.h"
#include "Codi/Renderer/Geometry.h"
#include "Codi/Renderer/Material.h"
#include "Codi/Renderer/Texture.h"

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

        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, const int32 entityID = -1);
        static void DrawQuad(const glm::mat4& transform, const Shared<Texture2D>& texture, const float32 tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const int32 entityID = -1);
        static void DrawQuad(const glm::mat4& transform, const Shared<Material>& material, const int32 entityID = -1);

        // Getters

        static Shared<Texture2D> GetTextureSlot(uint32 index);

        // Internals

        static void ResetStatistics();
        static Statistics GetStatistics();

    private:
        static void FlushAndReset();
    };

} // namespace Codi
