#include "codipch.h"
#include "Geometry.h"

namespace Codi {
    Geometry::Geometry(const std::string& name, Type type, const std::vector<GeometryVertex>& vertices, const std::vector<uint32>& indices, const Shared<Material>& material)
        : _Name(name)
        , _Type(type)
        , _Vertices(vertices)
        , _Indices(indices)
        , _Material(material)
        {
        UploadToGPU();
    }

    Geometry::Geometry(const std::string& name, Type type, const Shared<Material>&material)
        : _Name(name)
        , _Type(type)
        , _Material(material)
        {}

    Geometry::~Geometry() {
        DestroyGPU();
    }

    void Geometry::UploadToGPU() {
        if (_Vertices.empty() || _Indices.empty()) return;

        _VertexArray = VertexArray::Create();

        uint32 verticesDataSize = (uint32)(_Vertices.size() * sizeof(GeometryVertex));
        Shared<VertexBuffer> vbo = VertexBuffer::Create(_Vertices.data(), verticesDataSize);
        vbo->SetLayout({
            { ShaderDataType::Float3, "a_Position"  },
            { ShaderDataType::Float3, "a_Normal"    },
            { ShaderDataType::Float2, "a_TexCoord"  },
            { ShaderDataType::Float4, "a_Color"     },
        });

        uint32 indicesDataSize = (uint32)_Indices.size();
        Shared<IndexBuffer> ib = IndexBuffer::Create(_Indices.data(), indicesDataSize);

        _VertexArray->AddVertexBuffer(vbo);
        _VertexArray->SetIndexBuffer(ib);
    }

    void Geometry::DestroyGPU() {
        _VertexArray.reset();
    }

    Shared<Geometry> Geometry::Create(const std::string& name, Type type, const std::vector<GeometryVertex>& vertices, const std::vector<uint32>& indices, const Shared<Material>& material) {
        return Share<Geometry>(name, type, vertices, indices, material);
    }

    Shared<Geometry> Geometry::Create(const std::string& name, Type type, const Shared<Material>& material) {
        Shared<Geometry> geom = Share<Geometry>(name, type, material);

        switch (type) {
        case Type::Quad:
            geom->GenerateQuad();
            break;
        case Type::Custom:
        case Type::None:
            // nothing to generate in this case
            break;
        }

        geom->UploadToGPU();
        return geom;
    }

    void Geometry::GenerateQuad() {
        _Vertices = {
            { { -0.5f, -0.5f, 0.0f }, { 0, 0, 1 }, { 0, 0 }, { 1, 1, 1, 1 } },
            { {  0.5f, -0.5f, 0.0f }, { 0, 0, 1 }, { 1, 0 }, { 1, 1, 1, 1 } },
            { {  0.5f,  0.5f, 0.0f }, { 0, 0, 1 }, { 1, 1 }, { 1, 1, 1, 1 } },
            { { -0.5f,  0.5f, 0.0f }, { 0, 0, 1 }, { 0, 1 }, { 1, 1, 1, 1 } }
        };
        _Indices = { 0, 1, 2, 2, 3, 0 };
    }

} // namespace Codi
