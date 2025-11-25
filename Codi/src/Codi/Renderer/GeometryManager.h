#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/Geometry.h"

#include <unordered_map>

namespace Codi {

    class GeometryManager {
    public:
        static void Init();
        static void Shutdown();

        static Shared<Geometry> Create(const std::string& name, Geometry::Type type, const std::vector<GeometryVertex>& vertices, const std::vector<uint32>& indices, const Shared<Material>& material);
        static Shared<Geometry> Create(const std::string& name, Geometry::Type type, const Shared<Material>& material);

        static bool Exists(const UUID& uuid);
        static Shared<Geometry> Get(const UUID& uuid);

        static bool Exists(const std::string& path);
        static Shared<Geometry> Get(const std::string& name);

    private:
        GeometryManager() = default;

        static std::unordered_map<UUID, Shared<Geometry>> _GeometriesByUUID;
        static std::unordered_map<std::string, Shared<Geometry>> _GeometriesByName;
    };

} // namespace Codi
