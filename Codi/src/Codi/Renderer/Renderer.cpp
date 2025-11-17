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

    void Renderer::DrawFrame(const RenderPacket& packet) {
        _RAPI->BeginFrame(packet.Deltatime);

        // nothing yet

        _RAPI->EndFrame(packet.Deltatime);
    }

} // namespace Codi
