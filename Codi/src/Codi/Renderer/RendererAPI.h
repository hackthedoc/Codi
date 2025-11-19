#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/VertexArray.h"

namespace Codi {

    class RendererAPI {
    public:
        enum class API {
            None = 0,
            Vulkan
        };

    public:
        virtual ~RendererAPI() = default;

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        /// @return Wether the current frame is being skipped or not
        virtual bool BeginFrame() = 0;
        /// @return Wether the current frame is being skipped or not
        virtual bool EndFrame() = 0;

        virtual void OnWindowResize(uint32 width, uint32 height) = 0;
        virtual void SetViewport(float32 x, float32 y, float32 width, float32 height) = 0;

        virtual void DrawIndexed(const Shared<VertexArray>& vertexArray, uint32 indexCount) = 0;

        virtual inline void* GetCurrentFrameBufferHandle() const = 0;

        static Owned<RendererAPI> Create();

        static inline API GetAPI() { return _API; }

    private:
        static API _API;
    };

} // namespace Codi
