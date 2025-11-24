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

    class GlobalUniformRegistry {
    public:
        struct RegisteredBufferInfo {
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkDeviceSize size = 0;
            void* mapped = nullptr; // optional
        };

    public:
        static GlobalUniformRegistry& Get() {
            static GlobalUniformRegistry s;
            return s;
        }

        void Register(uint32 set, uint32 binding, const RegisteredBufferInfo& info) {
            std::lock_guard<std::mutex> lock(_mutex);
            _map[{set, binding}] = info;
        }

        // returns true if found
        bool Get(uint32 set, uint32 binding, RegisteredBufferInfo& out) {
            std::lock_guard<std::mutex> lock(_mutex);
            auto it = _map.find({ set, binding });
            if (it == _map.end()) return false;
            out = it->second;
            return true;
        }

        void Unregister(uint32 set, uint32 binding) {
            std::lock_guard<std::mutex> lock(_mutex);
            _map.erase({ set, binding });
        }

    private:
        GlobalUniformRegistry() = default;
        ~GlobalUniformRegistry() = default;

        std::unordered_map<UboKey, RegisteredBufferInfo, UboKeyHash> _map;
        std::mutex _mutex;
    };

} // namespace Codi
