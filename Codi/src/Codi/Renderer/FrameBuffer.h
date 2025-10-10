#pragma once

namespace Codi {

enum class FrameBufferTextureFormat {
    None = 0,

    // Colors

    RGBA8,

    // Depth/stencil

    DEPTH24STENCIL8,

    // Defaults

    Depth = DEPTH24STENCIL8
};

// TODO: filtering/wrap
struct FrameBufferTextureSpecification {
    FrameBufferTextureFormat textureFormat = FrameBufferTextureFormat::None;

    FrameBufferTextureSpecification() = default;
    FrameBufferTextureSpecification(FrameBufferTextureFormat format) : textureFormat(format) {}
};

struct FrameBufferAttachementSpecification {
    std::vector<FrameBufferTextureSpecification> attachements;

    FrameBufferAttachementSpecification() = default;
    FrameBufferAttachementSpecification(std::initializer_list<FrameBufferTextureSpecification> attachements) : attachements(attachements) {}
};

struct FrameBufferSpecification {
    uint32_t width;
    uint32_t height;
    FrameBufferAttachementSpecification attachements;
    uint32_t samples = 1;

    bool swapChainTarget = false;
};

class FrameBuffer {
public:
    virtual ~FrameBuffer() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual const FrameBufferSpecification& getSpecification() const = 0;

    virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const = 0;

    static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
};

} // namespace Codi
