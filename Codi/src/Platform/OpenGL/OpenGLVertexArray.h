#pragma once

#include "Codi/Renderer/VertexArray.h"
#include "Codi/Renderer/Buffer.h"

namespace Codi {

class OpenGLVertexArray : public VertexArray {
public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
    virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
    
    virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const override { return _vertexBuffers; }
    virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const override { return _indexBuffer; }

    inline uint32_t getID() const override { return _rendererID; }
private:
    std::vector<std::shared_ptr<VertexBuffer>> _vertexBuffers;
    std::shared_ptr<IndexBuffer> _indexBuffer;
    uint32_t _rendererID;
};

} // namespace Codi
