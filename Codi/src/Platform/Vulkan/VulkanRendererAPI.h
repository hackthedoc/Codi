#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/RendererAPI.h"

namespace Codi {

    class VulkanRendererAPI : public RendererAPI {
    public:
        virtual void Init() override;
        virtual void Shutdown() override;
    };

} // namespace Codi
