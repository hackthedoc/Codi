#include "cdpch.h"
#include "VertexArray.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Codi {

VertexArray* VertexArray::Create() {
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None  :
        CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return new OpenGLVertexArray();
    }

    CODI_CORE_ASSERT(false, "Unknown RenderAPI!");
    return nullptr;
}

} // namespace Codi
