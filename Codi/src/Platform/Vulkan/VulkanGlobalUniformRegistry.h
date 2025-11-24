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
            size_t h1 = std::hash<uint32>{}(k.first);
            size_t h2 = std::hash<uint32>{}(k.second);
            return h1 ^ (h2 << 1);
        }
    };

    struct UboKeyEqual {
        bool operator()(const UboKey& a, const UboKey& b) const noexcept {
            return a.first == b.first && a.second == b.second;
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
            UboKey key(set, binding);
            _Map[key] = info;
        }

        // returns true if found
        bool Get(uint32 set, uint32 binding, RegisteredBufferInfo& out) {
            std::lock_guard<std::mutex> lock(_Mutex);
            UboKey key(set, binding);
            auto it = _Map.find(key);
            if (it == _Map.end()) return false;
            out = it->second;
            return true;
        }

        void Unregister(uint32 set, uint32 binding) {
            std::lock_guard<std::mutex> lock(_Mutex);
            UboKey key(set, binding);
            auto it = _Map.find(key);
            if (it != _Map.end()) {
                it->second = {}; // optional: zero the buffer info
                _Map.erase(it);
            }
        }

    private:
        VulkanGlobalUniformRegistry() = default;
        ~VulkanGlobalUniformRegistry() = default;

        std::unordered_map<UboKey, RegisteredBufferInfo, UboKeyHash, UboKeyEqual> _Map;
        std::mutex _Mutex;
    };

} // namespace Codi
