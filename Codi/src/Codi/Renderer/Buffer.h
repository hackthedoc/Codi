#pragma once

#include "Codi/Core.h"

namespace Codi {

enum class ShaderDataType {
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool
};

static uint ShaderDataTypeSize(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::Float  : return 4;
    case ShaderDataType::Float2 : return 4 * 2;
    case ShaderDataType::Float3 : return 4 * 3;
    case ShaderDataType::Float4 : return 4 * 4;
    case ShaderDataType::Mat3   : return 4 * 3 * 3;
    case ShaderDataType::Mat4   : return 4 * 4 * 4;
    case ShaderDataType::Int    : return 4;
    case ShaderDataType::Int2   : return 4 * 2;
    case ShaderDataType::Int3   : return 4 * 3;
    case ShaderDataType::Int4   : return 4 * 4;
    case ShaderDataType::Bool   : return 1;
    }

    CODI_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

struct BufferElement {
    std::string name;
    ShaderDataType type;
    uint size;
    uint offset;
    bool normalized;

    BufferElement() {}

    BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
        : name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

    uint getComponentCount() const {
        switch (type) {
        case ShaderDataType::Float  : return 1;
        case ShaderDataType::Float2 : return 2;
        case ShaderDataType::Float3 : return 3;
        case ShaderDataType::Float4 : return 4;
        case ShaderDataType::Mat3   : return 3 * 3;
        case ShaderDataType::Mat4   : return 4 * 4;
        case ShaderDataType::Int    : return 1;
        case ShaderDataType::Int2   : return 2;
        case ShaderDataType::Int3   : return 3;
        case ShaderDataType::Int4   : return 4;
        case ShaderDataType::Bool   : return 1;
        }

        CODI_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }
};

class BufferLayout {
public:
    BufferLayout() {}

    BufferLayout(const std::initializer_list<BufferElement>& elements) : _elements(elements) {
        CalculateOffsetsAndStride();
    }

    inline uint getStride() const { return _stride; }

    inline const std::vector<BufferElement>& getElements() const { return _elements; }

    inline std::vector<BufferElement>::iterator begin() { return _elements.begin(); }
    inline std::vector<BufferElement>::iterator end() { return _elements.end(); }

private:
    void CalculateOffsetsAndStride() {
        uint offset = 0;
        _stride = 0;
        for (BufferElement& element : _elements) {
            element.offset = offset;
            offset += element.size;
            _stride += element.size;
        }
    }

    std::vector<BufferElement> _elements;
    uint _stride = 0;
};
    
class VertexBuffer {
public:
    virtual ~VertexBuffer() {}

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual const BufferLayout& getLayout() const = 0;
    virtual void setLayout(const BufferLayout& layout) = 0;

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


