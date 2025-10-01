#pragma once

namespace Codi {

struct FrameBufferSpecification {
    uint32_t width;
    uint32_t height;
    uint32_t samples = 1;

    bool swapChainTarget = false;
};

class FrameBuffer {
public:
    virtual ~FrameBuffer() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void resize(uint32_t width, uint32_t height) = 0;

    virtual const FrameBufferSpecification& getSpecificationRef() const = 0;

    virtual uint32_t getColorAttachmentRendererID() const = 0;

    static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
};

} // namespace Codi
