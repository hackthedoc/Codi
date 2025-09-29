#pragma once

#include "Codi/Renderer/RenderCommand.h"
#include "Codi/Renderer/OrthographicCamera.h"
#include "Codi/Renderer/Shader.h"

namespace Codi {

class Renderer {
public:
    static void BeginScene(OrthographicCamera& camera);
    static void EndScene();

    static void Submit(const Ref<class Shader>& shader, const Ref<class VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

    static void Init();

    inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

private:
    struct SceneData {
        glm::mat4 viewProjectionMatrix;
    };

    static SceneData* _sceneData;
};

} // namespace Codi
