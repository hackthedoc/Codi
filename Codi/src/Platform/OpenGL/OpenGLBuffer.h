#pragma once

#include "Codi/Renderer/Buffer.h"

namespace Codi {
    
class OpenGLVertexBuffer : public VertexBuffer {
public:
    OpenGLVertexBuffer(float* vertices, uint size);
    virtual ~OpenGLVertexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual const BufferLayout& getLayout() const override { return _layout; }
    virtual void setLayout(const BufferLayout& layout) override { _layout = layout; }

private:
    uint _rendererID;
    BufferLayout _layout;
};

class OpenGLIndexBuffer : public IndexBuffer {
public:
    OpenGLIndexBuffer(uint* indices, uint count);
    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;
    
    virtual uint getCount() const override { return _count; }

private:
    uint _rendererID;
    uint _count;
};

} // namespace Codi
