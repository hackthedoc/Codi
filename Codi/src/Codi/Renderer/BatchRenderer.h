#pragma once

#include "Codi/Core/Core.h"

#include "Codi/Renderer/Geometry.h"
#include "Codi/Renderer/Material.h"
#include "Codi/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Codi {

    class QuadBatchRenderer {
    public:
        static void Init();
        static void Shutdown();

        static void Flush();
        static void StartBatch();
        static void FlushAndReset();

        static void Submit(const glm::mat4& transform, const glm::vec4& color, const int32 entityID = -1);
        static void Submit(const glm::mat4& transform, const Shared<Texture2D>& texture, const float32 tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const int32 entityID = -1);
        static void Submit(const glm::mat4& transform, const Shared<Material>& material, const int32 entityID = -1);

        static Shared<Texture2D> GetTextureSlot(uint32 index);

    private:
        static float32 GetTextureIndex(const Shared<Texture2D>& texture);
    };

} // namespace Codi
