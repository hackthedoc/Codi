#pragma once

#include "Codi/Renderer/Buffer.h"

#include <vulkan/vulkan.h>

namespace Codi {

    class VulkanVertexBuffer : public VertexBuffer {
    public:
        VulkanVertexBuffer(uint32 size);
        VulkanVertexBuffer(void* vertices, uint32 size);
        virtual ~VulkanVertexBuffer();

        virtual void SetData(const void* data, uint32 size) override;

        virtual void* GetHandle() override { return &_Handle; }

        virtual const BufferLayout& GetLayout() const override { return _Layout; }
        virtual void SetLayout(const BufferLayout& layout) override { _Layout = layout; }

    private:
        VkBuffer _Handle = VK_NULL_HANDLE;
        VkDeviceMemory _Memory = VK_NULL_HANDLE;
        BufferLayout _Layout;
    };

    class VulkanIndexBuffer : public IndexBuffer {
    public:
        VulkanIndexBuffer(uint32* indices, uint32 count);
        virtual ~VulkanIndexBuffer();

        virtual void* GetHandle() override { return &_Handle; }

        virtual inline uint32 GetCount() const override { return _Count; }

    private:
        void CreateBuffer(uint32 size);
        void Upload(const void* data, uint32 size) const;

    private:
        VkBuffer _Handle = VK_NULL_HANDLE;
        VkDeviceMemory _Memory = VK_NULL_HANDLE;
        uint32 _Count;
    };

    class VulkanUniformBuffer : public UniformBuffer {
    public:
        VulkanUniformBuffer(uint32 size, uint32 binding);
        ~VulkanUniformBuffer();

        virtual void SetData(const void* data, uint32 size, uint32 offset = 0) override;

    private:
        VkBuffer _Handle = VK_NULL_HANDLE;
        VkDeviceMemory _Memory = VK_NULL_HANDLE;
        uint32 _Binding = 0;
        uint32 _Size = 0;
    };

} // namespace Codi
