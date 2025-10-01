#include "cdpch.h"
#include "Buffer.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Codi {
    
Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) {
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None  :
        CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return CreateRef<OpenGLVertexBuffer>(size);
    }

    CODI_CORE_ASSERT(false, "Unknown RenderAPI!");
    return nullptr;
}
    
Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint size) {
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None  :
        CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return CreateRef<OpenGLVertexBuffer>(vertices, size);
    }

    CODI_CORE_ASSERT(false, "Unknown RenderAPI!");
    return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::None  :
        CODI_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL:
        return CreateRef<OpenGLIndexBuffer>(indices, count);
    }

    CODI_CORE_ASSERT(false, "Unknown RenderAPI!");
    return nullptr;
}

} // namespace Codi