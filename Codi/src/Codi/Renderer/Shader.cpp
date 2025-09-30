#include "cdpch.h"
#include "Shader.h"

#include "Codi/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Codi {

Shader* Shader::Create(const std::string& filepath) {
    switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLShader(filepath);
    }

    CODI_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc) {
    switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLShader(vertexSrc, fragmentSrc);
    }

    CODI_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Codi