#include "codipch.h"
#include "Renderer.h"

#include "Codi/Renderer/Renderer2D.h"
#include "Codi/Renderer/GeometryManager.h"
#include "Codi/Renderer/MaterialManager.h"
#include "Codi/Renderer/TextureManager.h"

namespace Codi {

    Owned<RendererAPI> Renderer::_RAPI = nullptr;

    bool Renderer::_FrameSkipped = false;

    void Renderer::Init() {
        _RAPI = RendererAPI::Create();
        _RAPI->Init();

        TextureManager::Init();
        MaterialManager::Init();
        GeometryManager::Init();

        Renderer2D::Init();
    }

    void Renderer::Shutdown() {
        GeometryManager::Shutdown();
        MaterialManager::Shutdown();
        TextureManager::Shutdown();

        Renderer2D::Shutdown();

        _RAPI->Shutdown();
        _RAPI = nullptr;
    }

    void Renderer::OnWindowResize(uint32 width, uint32 height) {
        _RAPI->OnWindowResize(width, height);
    }

    void Renderer::BeginFrame() {
        _FrameSkipped = _RAPI->BeginFrame();
    }

    void Renderer::EndFrame() {
        if (_FrameSkipped) return;

        _RAPI->EndFrame();
    }

    void Renderer::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance) {
        if (_FrameSkipped) return;

        _RAPI->DrawIndexed(vertexArray, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

} // namespace Codi
