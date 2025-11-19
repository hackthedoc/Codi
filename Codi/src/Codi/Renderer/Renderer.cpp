#include "codipch.h"
#include "Renderer.h"

#include "Codi/Renderer/Renderer2D.h"

namespace Codi {

    Owned<RendererAPI> Renderer::_RAPI = nullptr;

    static bool FrameSkipped = false;

    void Renderer::Init() {
        _RAPI = RendererAPI::Create();
        _RAPI->Init();

        Renderer2D::Init();
    }

    void Renderer::Shutdown() {
        Renderer2D::Shutdown();

        _RAPI->Shutdown();
        _RAPI = nullptr;
    }

    void Renderer::OnWindowResize(uint32 width, uint32 height) {
        _RAPI->OnWindowResize(width, height);
    }

    void Renderer::BeginFrame() {
        FrameSkipped = _RAPI->BeginFrame();
    }

    void Renderer::EndFrame() {
        if (FrameSkipped) return;

        _RAPI->EndFrame();
    }

    void Renderer::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32 indexCount) {
        if (FrameSkipped) return;

        _RAPI->DrawIndexed(vertexArray, indexCount);
    }

} // namespace Codi
