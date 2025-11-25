#include "codipch.h"
#include "Renderer2D.h"

#include "Codi/Renderer/Renderer.h"
#include "Codi/Renderer/Shader.h"
#include "Codi/Renderer/VertexArray.h"
#include "Codi/Renderer/TextureManager.h"

namespace Codi {
    std::unordered_map<Geometry::Type, Owned<IBatchRenderer>> Renderer2D::_Renderers;

    struct Renderer2DData {
        struct CameraData {
            glm::mat4 ViewProjection = glm::mat4(0.0f);
        };
        CameraData CameraBuffer{};
        Shared<UniformBuffer> CameraUniformBuffer;

        Renderer2D::Statistics Stats;
    };
    static Renderer2DData Data;

    void Renderer2D::Init() {
        Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

        auto quadPlugin = Own<QuadBatchRendererPlugin>();
        RegisterRenderer(Geometry::Type::Quad, std::move(quadPlugin));
    }
    
    void Renderer2D::Shutdown() {
        Data.CameraUniformBuffer.reset();

        for (auto& [type, renderer] : _Renderers) {
            if (renderer) {
                renderer->Shutdown();
                renderer.reset();
            }
        }
        _Renderers.clear();
    }

    void Renderer2D::BeginScene(const EditorCamera& camera) {
        ResetStatistics();

        for (auto& [type, renderer] : _Renderers)
            renderer->StartBatch();

        Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
        Data.CameraUniformBuffer->SetData(&Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
    }

    void Renderer2D::EndScene() {
        for (auto& [type, renderer] : _Renderers)
            renderer->Flush();
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, const int32 entityID) {
        QuadBatchRenderer::Submit(transform, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Shared<Material>& material, const int32 entityID) {
        Geometry::Type type = Geometry::Type::Quad;

        auto it = _Renderers.find(type);
        if (it == _Renderers.end() || !(it->second)) {
            // fallback to quad renderer if present
            auto fallback = _Renderers.find(Geometry::Type::Quad);
            if (fallback != _Renderers.end() && fallback->second) {
                fallback->second->Submit(transform, nullptr, material, entityID);
                Data.Stats.QuadCount++;
                return;
            }
            // nothing registered for this type
            return;
        }

        it->second->Submit(transform, nullptr, material, entityID);

        // increase basic stats for known types (you can extend per type)
        if (type == Geometry::Type::Quad) Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawGeometry(const glm::mat4& transform, const Shared<Geometry>& geometry, const int32 entityID) {
        Geometry::Type type = Geometry::Type::Custom;
        if (geometry) type = geometry->GetType();
        else type = Geometry::Type::Quad; // treat no-geometry as a Quad primitive

        auto it = _Renderers.find(type);
        if (it == _Renderers.end() || !(it->second)) {
            // fallback to quad renderer if present
            auto fallback = _Renderers.find(Geometry::Type::Quad);
            if (fallback != _Renderers.end() && fallback->second) {
                fallback->second->Submit(transform, geometry, geometry->GetMaterial(), entityID);
                Data.Stats.QuadCount++;
                return;
            }
            // nothing registered for this type
            return;
        }

        it->second->Submit(transform, geometry, geometry->GetMaterial(), entityID);

        // increase basic stats for known types (you can extend per type)
        if (type == Geometry::Type::Quad) Data.Stats.QuadCount++;
    }

    Shared<Texture2D> Renderer2D::GetTextureSlot(uint32 index) {
        return QuadBatchRenderer::GetTextureSlot(index);
    }

    void Renderer2D::ResetStatistics() {
        Data.Stats.Reset();
    }

    Renderer2D::Statistics Renderer2D::GetStatistics() {
        return Data.Stats;
    }

    void Renderer2D::RegisterRenderer(Geometry::Type type, Owned<IBatchRenderer> renderer) {
        if (!renderer) return;
        // If a renderer for this type already exists, shut it down first
        auto it = _Renderers.find(type);
        if (it != _Renderers.end() && it->second) {
            it->second->Shutdown();
            it->second.reset();
        }
        renderer->Init();
        _Renderers[type] = std::move(renderer);
    }

    void Renderer2D::UnregisterRenderer(Geometry::Type type) {
        auto it = _Renderers.find(type);
        if (it == _Renderers.end()) return;
        if (it->second) {
            it->second->Flush();
            it->second->Shutdown();
            it->second.reset();
        }
        _Renderers.erase(it);
    }

} // namespace Codi
