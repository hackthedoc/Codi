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

    void Renderer::OnWindowResize(uint32 width, uint32 height) {
        _RAPI->OnWindowResize(width, height);
    }

    void Renderer::DrawFrame(const RenderPacket& packet) {
        bool frameSkipped = _RAPI->BeginFrame(packet.Deltatime);
        if (frameSkipped) return;

        // nothing yet

        _RAPI->EndFrame(packet.Deltatime);
    }

} // namespace Codi
