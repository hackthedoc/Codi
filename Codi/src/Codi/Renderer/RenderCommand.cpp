#include "cdpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Codi {

RendererAPI* RenderCommand::_rendererAPI = new OpenGLRendererAPI();

} // namespace Codi
