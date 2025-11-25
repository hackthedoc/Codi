#include "codipch.h"
#include "VulkanTexture.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanUtils.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

#include <stb_image.h>

namespace Codi {
    VulkanTexture2D::VulkanTexture2D(uint32 width, uint32 height, void* data) : _Width(width), _Height(height) {
        UploadToGPU(data);
    }

    VulkanTexture2D::VulkanTexture2D(const std::string& path) : _Path(path) {
        int32 texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        CODI_CORE_ASSERT(pixels, "Failed to load texture image!");

        _Width = texWidth;
        _Height = texHeight;

        UploadToGPU(pixels);

        stbi_image_free(pixels);
    }

    void VulkanTexture2D::Destroy() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice device = api.GetContext()->GetLogicalDevice();

        if (_Sampler)
            vkDestroySampler(device, _Sampler, VulkanRendererAPI::GetAllocator());

        if (_ImageView)
            vkDestroyImageView(device, _ImageView, VulkanRendererAPI::GetAllocator());

        if (_Image)
            vkDestroyImage(device, _Image, VulkanRendererAPI::GetAllocator());

        if (_Memory)
            vkFreeMemory(device, _Memory, VulkanRendererAPI::GetAllocator());

        _Image = VK_NULL_HANDLE;
        _Memory = VK_NULL_HANDLE;
        _ImageView = VK_NULL_HANDLE;
        _Sampler = VK_NULL_HANDLE;
    }

    void VulkanTexture2D::SetData(void* data, uint32 size) {
        UploadToGPU(data);
    }

    void VulkanTexture2D::Bind(uint32 slot) const { /* no-op */ }

    void VulkanTexture2D::Unbind() const { /* no-op */ }

    void VulkanTexture2D::UploadToGPU(void* pixels) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        Utils::CreateVulkanImage(_Width, _Height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, &_Image, &_Memory);
        Utils::UploadToVulkanImage(_Image, _Width, _Height, pixels);
        _ImageView = Utils::CreateVulkanImageView(_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

        VkSamplerCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        // TODO: make customizable
        info.magFilter = VK_FILTER_LINEAR; 
        info.minFilter = VK_FILTER_LINEAR;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        vkCreateSampler(logicalDevice, &info, VulkanRendererAPI::GetAllocator(), &_Sampler);

        _DescriptorInfo.sampler = _Sampler;
        _DescriptorInfo.imageView = _ImageView;
        _DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

} // namespace Codi
