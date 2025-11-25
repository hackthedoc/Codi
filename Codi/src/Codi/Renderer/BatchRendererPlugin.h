#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Core/UUID.h"
#include "Codi/Renderer/Geometry.h"
#include "Codi/Renderer/Material.h"
#include "Codi/Renderer/EditorCamera.h"

#include "Codi/Renderer/BatchRenderer.h"

#include <glm/glm.hpp>

namespace Codi {

    class IBatchRenderer {
    public:
        virtual ~IBatchRenderer() = default;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        virtual void StartBatch() = 0;
        virtual void Flush() = 0;

        /// @brief Submit calls - geometry can be optional for procedurals
        /// @param transform : model matrix
        /// @param geometry : optional geometry object (can be null) 
        /// @param material : material used to shade the primitive 
        /// @param entityID : editor/entity id (optional) 
        virtual void Submit(const glm::mat4& transform, const Shared<Geometry>& geometry, const Shared<Material>& material, int32 entityID) = 0;

        // Which geometry types does this renderer handle?
        virtual Geometry::Type GetSupportedType() const = 0;
    };

    class QuadBatchRendererPlugin : public IBatchRenderer {
    public:
        QuadBatchRendererPlugin() = default;
        virtual ~QuadBatchRendererPlugin() = default;

        virtual void Init() override { QuadBatchRenderer::Init(); }
        virtual void Shutdown() override { QuadBatchRenderer::Shutdown(); }

        virtual void StartBatch() override { QuadBatchRenderer::StartBatch(); }
        virtual void Flush() override { QuadBatchRenderer::Flush(); }

        virtual void Submit(const glm::mat4& transform, const Shared<Geometry>&, const Shared<Material>& material, int32 entityID) override {
            // our QuadBatchRenderer::Submit expects (transform, material, entityID) and treats null geometry as quad
            QuadBatchRenderer::Submit(transform, material, entityID);
        }

        virtual Geometry::Type GetSupportedType() const override { return Geometry::Type::Quad; }
    };

} // namespace Codi
