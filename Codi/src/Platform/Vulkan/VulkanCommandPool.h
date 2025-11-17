#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Codi {

    struct VulkanCommandPoolSpecification {
        uint32 QueueFamilyIndex = 0;
        bool ResetCommandBuffer = true;
        bool Transient = false;
    };

    class VulkanCommandPool {
    public:
        VulkanCommandPool() = default;
        VulkanCommandPool(const VulkanCommandPoolSpecification& spec);
        ~VulkanCommandPool() = default;

        void Create();
        void Destroy();
        void ResetPool() const;

        inline VkCommandPool GetHandle() const { return _Handle; }
        inline const VulkanCommandPoolSpecification& GetSpecification() const { return _Specification; }

    private:
        VkCommandPool _Handle;
        VulkanCommandPoolSpecification _Specification{};
    };

} // namespace Codi
