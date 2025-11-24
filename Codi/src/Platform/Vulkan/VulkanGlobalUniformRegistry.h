#pragma once

#include "Codi/Core/Core.h"

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <mutex>

namespace Codi {

    // Key: (set, binding)
    using UboKey = std::pair<uint32, uint32>;

    struct UboKeyHash {
        uint64 operator()(UboKey const& k) const noexcept {
            return (uint64(k.first) << 32) ^ uint64(k.second);
        }
    };

    class VulkanGlobalUniformRegistry {
    public:
        struct RegisteredBufferInfo {
            VkBuffer Buffer = VK_NULL_HANDLE;
            VkDeviceMemory Memory = VK_NULL_HANDLE;
            VkDeviceSize Size = 0;
            void* Mapped = nullptr; // optional
        };

    public:
        static VulkanGlobalUniformRegistry& Get() {
            static VulkanGlobalUniformRegistry s;
            return s;
        }

        void Register(uint32 set, uint32 binding, const RegisteredBufferInfo& info) {
            std::lock_guard<std::mutex> lock(_Mutex);
            _Map[{set, binding}] = info;
        }

        // returns true if found
        bool Get(uint32 set, uint32 binding, RegisteredBufferInfo& out) {
            std::lock_guard<std::mutex> lock(_Mutex);
            auto it = _Map.find({ set, binding });
            if (it == _Map.end()) return false;
            out = it->second;
            return true;
        }

        void Unregister(uint32 set, uint32 binding) {
            std::lock_guard<std::mutex> lock(_Mutex);
            _Map.erase({ set, binding });
        }

    private:
        VulkanGlobalUniformRegistry() = default;
        ~VulkanGlobalUniformRegistry() = default;

        std::unordered_map<UboKey, RegisteredBufferInfo, UboKeyHash> _Map;
        std::mutex _Mutex;
    };

} // namespace Codi
