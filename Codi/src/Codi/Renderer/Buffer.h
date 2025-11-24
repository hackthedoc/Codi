#pragma once

#include "Codi/Core/Core.h"

#include <vector>

namespace Codi {

    enum class ShaderDataType {
        None = 0,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Int, Int2, Int3, Int4,
        Bool
    };

    static uint32 ShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
        case ShaderDataType::Float: return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::Mat3: return 4 * 3 * 3;
        case ShaderDataType::Mat4: return 4 * 4 * 4;
        case ShaderDataType::Int: return 4;
        case ShaderDataType::Int2: return 4 * 2;
        case ShaderDataType::Int3: return 4 * 3;
        case ShaderDataType::Int4: return 4 * 4;
        case ShaderDataType::Bool: return 1;
        }

        CODI_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    struct BufferElement {
        std::string Name;
        ShaderDataType Type;
        uint32 Size;
        uint32 Offset;
        bool Normalized;

        BufferElement()
            : Name(""), Type(ShaderDataType::Bool), Size(1), Offset(0), Normalized(true)
            {}

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
            {}

        uint8 GetComponentCount() const {
            switch (Type) {
            case ShaderDataType::Float: return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::Mat3: return 3 * 3;
            case ShaderDataType::Mat4: return 4 * 4;
            case ShaderDataType::Int: return 1;
            case ShaderDataType::Int2: return 2;
            case ShaderDataType::Int3: return 3;
            case ShaderDataType::Int4: return 4;
            case ShaderDataType::Bool: return 1;
            }

            CODI_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    };

    class BufferLayout {
    public:
        BufferLayout() = default;

        BufferLayout(const std::initializer_list<BufferElement>& elements) : _Elements(elements) {
            CalculateOffsetsAndStride();
        }

        inline uint32 GetStride() const { return _Stride; }

        inline const std::vector<BufferElement>& GetElements() const { return _Elements; }

        inline std::vector<BufferElement>::iterator begin() { return _Elements.begin(); }
        inline std::vector<BufferElement>::iterator end() { return _Elements.end(); }

    private:
        void CalculateOffsetsAndStride() {
            uint32 offset = 0;
            _Stride = 0;
            for (BufferElement& element : _Elements) {
                element.Offset = offset;
                offset += element.Size;
                _Stride += element.Size;
            }
        }

        std::vector<BufferElement> _Elements;
        uint32 _Stride = 0;
    };

    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;

        virtual void SetData(const void* data, uint32 size) = 0;

        virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout& layout) = 0;

        virtual void* GetHandle() = 0;

        static Shared<VertexBuffer> Create(uint32 size);
        static Shared<VertexBuffer> Create(void* vertices, uint32 size);
    };

    /// @brief IndexBuffer supports only 32-bit unsigned int indices.
    class IndexBuffer {
    public:
        virtual ~IndexBuffer() = default;

        virtual void* GetHandle() = 0;

        virtual inline uint32 GetCount() const = 0;

        static Shared<IndexBuffer> Create(uint32* indices, uint32 count);
    };

    class UniformBuffer {
    public:
        virtual ~UniformBuffer() = default;

        virtual void SetData(const void* data, uint32 size, uint32 offset = 0) = 0;

        static Shared<UniformBuffer> Create(uint32 size, uint32 binding);
    };

    class ShaderStorageBuffer {
    public:
        virtual ~ShaderStorageBuffer() = default;

        virtual void SetData(const void* data, uint32 size, uint32 offset = 0) = 0;

        static Shared<ShaderStorageBuffer> Create(uint32 size, uint32 binding);
    };

} // namespace Codi
