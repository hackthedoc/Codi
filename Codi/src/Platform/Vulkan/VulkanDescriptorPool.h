#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Codi {

    class VulkanDescriptorPool {
    public:
        VulkanDescriptorPool() = default;
        ~VulkanDescriptorPool() = default;

        void Create();
        void Destroy();

        VkDescriptorPool GetHandle() const { return _Handle; }

    private:
        VkDescriptorPool _Handle = VK_NULL_HANDLE;
    };

} // namespace Codi
