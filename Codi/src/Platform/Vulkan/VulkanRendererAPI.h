#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanGraphicsContext.h"

#include <vulkan/vulkan.h>

namespace Codi {

    class VulkanRendererAPI : public RendererAPI {
    public:
        virtual void Init() override;
        virtual void Shutdown() override;

        static VkAllocationCallbacks* GetAllocator() { return nullptr; }

    private:
        Owned<VulkanGraphicsContext> _Context = nullptr;
    };

} // namespace Codi
