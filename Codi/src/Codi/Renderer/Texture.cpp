#include "cdpch.h"
#include "Texture.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"


namespace Codi {

Ref<Texture2D> Texture2D::Create(const std::string& path) {
    switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLTexture2D>(path);
    }

    CODI_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Codi
