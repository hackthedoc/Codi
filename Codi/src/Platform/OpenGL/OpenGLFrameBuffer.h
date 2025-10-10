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

    virtual const FrameBufferSpecification& getSpecification() const override { return _specification; }

    virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const override { CODI_CORE_ASSERT(index < _colorAttachments.size()); return _colorAttachments[index]; }

private:
    FrameBufferSpecification _specification;
    uint32_t _rendererID = 0;

    std::vector<FrameBufferTextureSpecification> _colorAttachmentSpecs;
    FrameBufferTextureSpecification _depthAttachementSpec;

    std::vector<uint32_t> _colorAttachments;
    uint32_t _depthAttachment = 0;
};

} // namespace Codi
