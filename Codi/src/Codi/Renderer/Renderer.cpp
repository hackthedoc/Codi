#include "codipch.h"
#include "Renderer.h"

namespace Codi {

    Owned<RendererAPI> Renderer::_RAPI = nullptr;

    void Renderer::Init() {
        _RAPI = RendererAPI::Create();
        _RAPI->Init();
    }

    void Renderer::Shutdown() {
        _RAPI->Shutdown();
        _RAPI = nullptr;
    }

    void Renderer::DrawFrame() {
        // nothing yet
    }

} // namespace Codi
