#pragma once

#include "Codi/Renderer/Camera.h"
#include "Codi/Renderer/EditorCamera.h"
#include "Codi/Renderer/OrthographicCamera.h" // TODO: Remove
#include "Codi/Renderer/SubTexture.h"
#include "Codi/Renderer/Texture.h"

namespace Codi {

class Renderer2D {
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const Camera& camera, const glm::mat4& transform);
    static void BeginScene(const EditorCamera& camera);
    static void BeginScene(const OrthographicCamera& camera); // TODO: Remove
    static void EndScene();
    static void Flush();

    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

    static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
    static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& color);
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color);
    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const Ref<SubTexture2D>& subtexture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const Ref<SubTexture2D>& subtexture, const float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

    
    struct Statistics {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;

        uint32_t GetTotalVertexCount() { return QuadCount * 4; }
        uint32_t GetTotalIndexCount() { return QuadCount * 6; }

        void Reset() {
            DrawCalls = 0;
            QuadCount = 0;
        }
    };

    static void ResetStats();
    static Statistics GetStats();

private:
    static void FlushAndReset();
};

} // namespace Codi