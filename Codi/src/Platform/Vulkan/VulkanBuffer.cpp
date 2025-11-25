#include "codipch.h"
#include "VulkanBuffer.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanUtils.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanGlobalUniformRegistry.h"

namespace Codi {

    // VERTEX BUFFER ----------------------------------------

    VulkanVertexBuffer::VulkanVertexBuffer(uint32 size) {
        Utils::CreateVulkanBuffer(
            size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            &_Handle,
            &_Memory
        );
    }

    VulkanVertexBuffer::VulkanVertexBuffer(void* vertices, uint32 size) {
        Utils::CreateVulkanBuffer(
            size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            &_Handle,
            &_Memory
        );
        Utils::UploadVulkanBuffer(vertices, size, 0, _Memory);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        Utils::DestroyVulkanBuffer(&_Handle, &_Memory);
    }

    void VulkanVertexBuffer::SetData(const void* data, uint32 size) {
        Utils::UploadVulkanBuffer(data, size, 0, _Memory);
    }

    // INDEX BUFFER -----------------------------------------

    VulkanIndexBuffer::VulkanIndexBuffer(uint32* indices, uint32 count) : _Count(count) {
        const uint32 size = count * sizeof(uint32);

        Utils::CreateVulkanBuffer(
            size,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            & _Handle,
            &_Memory
        );
        Utils::UploadVulkanBuffer(indices, size, 0, _Memory);
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        Utils::DestroyVulkanBuffer(&_Handle, &_Memory);
    }

    // UNIFORM BUFFER ---------------------------------------

    VulkanUniformBuffer::VulkanUniformBuffer(uint32 size, uint32 binding) : _Size(size), _Binding(binding) {
        Utils::CreateVulkanBuffer(
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            &_Handle,
            &_Memory
        );

        // Map persistently ? (we keep previous behaviour, we don't persistently map here)
        // Query properties so registry can store size and mapped pointer (null for now)
        VulkanGlobalUniformRegistry::RegisteredBufferInfo info;
        info.Buffer = _Handle;
        info.Memory = _Memory;
        info.Size = _Size;
        info.Mapped = nullptr;

        // register for set=0
        VulkanGlobalUniformRegistry::Get().Register(0, _Binding, info);
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        // Unregisterr before destroy
        VulkanGlobalUniformRegistry::Get().Unregister(0, _Binding);
        Utils::DestroyVulkanBuffer(&_Handle, &_Memory);
    }

    void VulkanUniformBuffer::SetData(const void* data, uint32 size, uint32 offset) {
        Utils::UploadVulkanBuffer(data, size, offset, _Memory);
    }

    // STORAGE BUFFER ---------------------------------------

    VulkanShaderStorageBuffer::VulkanShaderStorageBuffer(uint32 size, uint32 binding) : _Size(size), _Binding(binding) {
        Utils::CreateVulkanBuffer(
            size,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            &_Handle,
            &_Memory
        );

        // Map persistently ? (we keep previous behaviour, we don't persistently map here)
        // Query properties so registry can store size and mapped pointer (null for now)
        VulkanGlobalUniformRegistry::RegisteredBufferInfo info;
        info.Buffer = _Handle;
        info.Memory = _Memory;
        info.Size = _Size;
        info.Mapped = nullptr;

        // register for set=0
        VulkanGlobalUniformRegistry::Get().Register(0, _Binding, info);
    }

    VulkanShaderStorageBuffer::~VulkanShaderStorageBuffer() {
        // Unregisterr before destroy
        VulkanGlobalUniformRegistry::Get().Unregister(0, _Binding);

        Utils::DestroyVulkanBuffer(&_Handle, &_Memory);
    }

    void VulkanShaderStorageBuffer::SetData(const void* data, uint32 size, uint32 offset) {
        Utils::UploadVulkanBuffer(data, size, offset, _Memory);
    }

} // namespace Codi
