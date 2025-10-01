#include "cdpch.h"
#include "FrameBuffer.h"

#include "Codi/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Codi {

Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec) {
    switch (RendererAPI::GetAPI()) {
        case RendererAPI::API::None:
            CODI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLFrameBuffer>(spec);
    }

    CODI_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Codi
