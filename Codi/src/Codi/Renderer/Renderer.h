#pragma once

#include "Codi/Renderer/RenderCommand.h"
#include "Codi/Renderer/OrthographicCamera.h"
#include "Codi/Renderer/Shader.h"

namespace Codi {

class Renderer {
public:
    static void BeginScene(OrthographicCamera& camera);
    static void EndScene();

    static void Submit(const std::shared_ptr<class Shader>& shader, const std::shared_ptr<class VertexArray>& vertexArray);

    inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

private:
    struct SceneData {
        glm::mat4 viewProjectionMatrix;
    };

    static SceneData* _sceneData;
};

} // namespace Codi
