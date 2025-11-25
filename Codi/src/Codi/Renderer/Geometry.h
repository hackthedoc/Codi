#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Core/UUID.h"
#include "Codi/Renderer/Material.h"
#include "Codi/Renderer/VertexArray.h"

#include <vector>

namespace Codi {

    struct GeometryVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
        glm::vec4 Color;
    };

    class Geometry {
    public:
        enum class Type {
            None = 0,
            Quad,
            Custom
        };

    public:
        Geometry(const std::string& name, Type type, const std::vector<GeometryVertex>& vertices, const std::vector<uint32>& indices, const Shared<Material>& material);
        Geometry(const std::string& name, Type type, const Shared<Material>& material);
        ~Geometry();

        const UUID& GetUUID() const { return _UUID; }
        const std::string& GetName() const { return _Name; }
        const Type GetType() const { return _Type; }

        const std::vector<GeometryVertex>& GetVertices() const { return _Vertices; }
        const std::vector<uint32>& GetIndices() const { return _Indices; }
        Shared<Material> GetMaterial() const { return _Material; }

        void UploadToGPU();
        void DestroyGPU();

        Shared<VertexArray> GetVertexArray() const { return _VertexArray; }

        static Shared<Geometry> Create(const std::string& name, Type type, const std::vector<GeometryVertex>& vertices, const std::vector<uint32>& indices, const Shared<Material>& material);
        static Shared<Geometry> Create(const std::string& name, Type type, const Shared<Material>& material = nullptr);

        bool operator==(const Geometry& other) const { return _UUID == other.GetUUID(); }

    private:
        void GenerateQuad();

    private:
        UUID _UUID;
        std::string _Name;
        Type _Type = Type::None;

        std::vector<GeometryVertex> _Vertices;
        std::vector<uint32> _Indices;
        Shared<Material> _Material;

        Shared<VertexArray> _VertexArray = nullptr;
    };

} // namespace Codi
