#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Codi {

    class VulkanFramebuffers {
    public:
        VulkanFramebuffers() = default;
        ~VulkanFramebuffers() = default;

        void Create();
        void Destroy();

        VkFramebuffer Get(uint32 index) const { return _Framebuffers[index]; }
        uint32 Size() const { return _Framebuffers.size(); }

        VkFramebuffer operator[](uint32 index) const { return Get(index); }

    private:
        std::vector<VkFramebuffer> _Framebuffers;
    };

} // namespace Codi
