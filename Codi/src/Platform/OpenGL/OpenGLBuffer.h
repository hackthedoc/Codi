#pragma once

#include "Codi/Renderer/Buffer.h"

namespace Codi {
    
class OpenGLVertexBuffer : public VertexBuffer {
public:
    OpenGLVertexBuffer(uint32_t size);
    OpenGLVertexBuffer(float* vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void setData(const void* data, uint32_t size) override;

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

private:
    uint _rendererID;
    uint32_t _count;
};

} // namespace Codi
