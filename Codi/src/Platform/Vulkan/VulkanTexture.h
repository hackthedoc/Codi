#pragma once

#include "Codi/Renderer/Texture.h"

#include <vulkan/vulkan.h>

namespace Codi {

    class VulkanTexture2D : public Texture2D {
    public:
        VulkanTexture2D(uint32 width, uint32 height, void* data = nullptr);
        VulkanTexture2D(const std::string& path);
        virtual ~VulkanTexture2D() = default;

        virtual void Destroy() override;

        virtual uint32 GetWidth() const override { return _Width; }
        virtual uint32 GetHeight() const override { return _Height; }
        virtual const std::string& GetPath() const override { return _Path; }

        virtual void SetData(void* data, uint32 size) override;

        virtual void Bind(uint32 slot = 0) const override;
        virtual void Unbind() const override;

        virtual bool operator==(const Texture& other) const override { return _Image == ((VulkanTexture2D&)other).GetImage(); }

        VkImage GetImage() const { return _Image; }
        VkDescriptorImageInfo& GetDescriptorInfo() const { return _DescriptorInfo; }

    private:
        void UploadToGPU(void* pixels);

    private:
        std::string _Path;
        uint32 _Width = 0;
        uint32 _Height = 0;

        VkImage _Image = VK_NULL_HANDLE;
        VkDeviceMemory _Memory = VK_NULL_HANDLE;
        VkImageView _ImageView = VK_NULL_HANDLE;
        VkSampler _Sampler = VK_NULL_HANDLE;

        mutable VkDescriptorImageInfo _DescriptorInfo{};
    };

} // namespace Codi
