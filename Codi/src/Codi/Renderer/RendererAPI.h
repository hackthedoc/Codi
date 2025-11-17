#pragma once

#include "Codi/Core/Core.h"

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

        static Owned<RendererAPI> Create();

        static inline API GetAPI() { return _API; }

    private:
        static API _API;
    };

} // namespace Codi
