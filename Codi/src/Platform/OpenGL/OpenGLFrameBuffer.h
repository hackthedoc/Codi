#pragma once

#include "Codi/Renderer/FrameBuffer.h"

namespace Codi {

class OpenGLFrameBuffer : public FrameBuffer {
public:
    OpenGLFrameBuffer(const FrameBufferSpecification& spec);
    virtual ~OpenGLFrameBuffer();

    void invalidate();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void resize(uint32_t width, uint32_t height) override;

    virtual const FrameBufferSpecification& getSpecificationRef() const override { return _specification; }

    virtual uint32_t getColorAttachmentRendererID() const override { return _colorAttachment; }

private:
    FrameBufferSpecification _specification;
    uint32_t _rendererID = 0;
    uint32_t _colorAttachment = 0;
    uint32_t _depthAttachment = 0;
};

} // namespace Codi
