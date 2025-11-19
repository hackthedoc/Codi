#include "codipch.h"
#include "VulkanBuffer.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    namespace Utils {

        static void CreateBuffer(uint32 size, VkBufferUsageFlags usage, VkBuffer* buffer, VkDeviceMemory* memory) {
            VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
            VulkanGraphicsContext* context = api.GetContext();
            VkDevice logicalDevice = context->GetLogicalDevice();

            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            vkCreateBuffer(logicalDevice, &bufferInfo, VulkanRendererAPI::GetAllocator(), buffer);

            VkMemoryRequirements memReq;
            vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memReq);

            VkMemoryAllocateInfo memInfo{};
            memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memInfo.allocationSize = memReq.size;
            memInfo.memoryTypeIndex = context->FindMemoryType(
                memReq.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            vkAllocateMemory(logicalDevice, &memInfo, VulkanRendererAPI::GetAllocator(), memory);
            vkBindBufferMemory(logicalDevice, *buffer, *memory, 0);
        }

        static void Upload(const void* data, uint32 size, VkDeviceMemory memory) {
            VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
            VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

            void* mapped;
            vkMapMemory(logicalDevice, memory, 0, size, 0, &mapped);
            memcpy(mapped, data, size);
            vkUnmapMemory(logicalDevice, memory);
        }

        static void DestroyBuffer(VkBuffer* buffer, VkDeviceMemory* memory) {
            VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
            VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

            vkDestroyBuffer(logicalDevice, *buffer, VulkanRendererAPI::GetAllocator());
            vkFreeMemory(logicalDevice, *memory, VulkanRendererAPI::GetAllocator());

            *buffer = VK_NULL_HANDLE;
            *memory = VK_NULL_HANDLE;
        }

    } // namespace Utils

    // VERTEX BUFFER ----------------------------------------

    VulkanVertexBuffer::VulkanVertexBuffer(uint32 size) {
        Utils::CreateBuffer(
            size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            &_Handle,
            &_Memory
        );
    }

    VulkanVertexBuffer::VulkanVertexBuffer(void* vertices, uint32 size) {
        Utils::CreateBuffer(
            size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            &_Handle,
            &_Memory
        );
        Utils::Upload(vertices, size, _Memory);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        Utils::DestroyBuffer(&_Handle, &_Memory);
    }

    void VulkanVertexBuffer::SetData(const void* data, uint32 size) {
        Utils::Upload(data, size, _Memory);
    }

    // INDEX BUFFER -----------------------------------------

    VulkanIndexBuffer::VulkanIndexBuffer(uint32* indices, uint32 count) : _Count(count) {
        const uint32 size = count * sizeof(uint32);

        Utils::CreateBuffer(
            size,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            & _Handle,
            &_Memory
        );
        Utils::Upload(indices, size, _Memory);
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        Utils::DestroyBuffer(&_Handle, &_Memory);
    }

    // UNIFORM BUFFER ---------------------------------------

    VulkanUniformBuffer::VulkanUniformBuffer(uint32 size, uint32 binding) : _Size(size), _Binding(binding) {
        Utils::CreateBuffer(
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            &_Handle,
            &_Memory
        );
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        Utils::DestroyBuffer(&_Handle, &_Memory);
    }

    void VulkanUniformBuffer::SetData(const void* data, uint32 size, uint32 offset) {
        Utils::Upload(data, size, _Memory);
    }

} // namespace Codi
