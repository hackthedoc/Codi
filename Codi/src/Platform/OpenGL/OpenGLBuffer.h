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
    OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
    virtual ~OpenGLIndexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;
    
    virtual uint32_t getCount() const override { return _count; }

    virtual uint getID() const override { return _rendererID; }

private:
    uint _rendererID;
    uint32_t _count;
};

} // namespace Codi
