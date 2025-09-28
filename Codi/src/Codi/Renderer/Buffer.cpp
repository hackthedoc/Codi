#include "cdpch.h"
#include "Buffer.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Codi {
    
VertexBuffer* VertexBuffer::Create(float* vertices, uint size) {
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None  :
        CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return new OpenGLVertexBuffer(vertices, size);
    }

    CODI_CORE_ASSERT(false, "Unknown RenderAPI!");
    return nullptr;
}

IndexBuffer* IndexBuffer::Create(uint* indices, uint size) {
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None  :
        CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return new OpenGLIndexBuffer(indices, size);
    }

    CODI_CORE_ASSERT(false, "Unknown RenderAPI!");
    return nullptr;
}

} // namespace Codi