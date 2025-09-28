#pragma once

#include "Codi/Core.h"

namespace Codi {
    
class VertexBuffer {
public:
    virtual ~VertexBuffer() {}

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    static VertexBuffer* Create(float* vertices, uint size);
};

class IndexBuffer {
public:
    virtual ~IndexBuffer() {}

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual uint getCount() const = 0;

    static IndexBuffer* Create(uint* indices, uint size);
};

} // namespace Codi


