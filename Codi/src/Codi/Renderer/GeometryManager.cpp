#include "codipch.h"
#include "GeometryManager.h"

namespace Codi {

    std::unordered_map<UUID, Shared<Geometry>> GeometryManager::_GeometriesByUUID;
    std::unordered_map<std::string, Shared<Geometry>> GeometryManager::_GeometriesByName;

    void GeometryManager::Init() {
        _GeometriesByUUID.clear();
        _GeometriesByName.clear();

        CODI_CORE_INFO("Geometry Manager initialized!");
    }

    void GeometryManager::Shutdown() {
        CODI_CORE_INFO("Cleaning Geometry Manager...");

        for (auto& [key, geometry] : _GeometriesByUUID)
            geometry->DestroyGPU();

        _GeometriesByUUID.clear();
        _GeometriesByName.clear();
    }

    Shared<Geometry> GeometryManager::Create(const std::string& name, Geometry::Type type, const std::vector<GeometryVertex>& vertices, const std::vector<uint32>& indices, const Shared<Material>& material) {
        if (Exists(name)) return Get(name);

        Shared<Geometry> geom = Geometry::Create(name, type, vertices, indices, material);
        _GeometriesByUUID[geom->GetUUID()] = geom;
        _GeometriesByName[name] = geom;
        return geom;
    }

    Shared<Geometry> GeometryManager::Create(const std::string& name, Geometry::Type type, const Shared<Material>& material) {
        if (Exists(name)) return Get(name);

        Shared<Geometry> geom = Geometry::Create(name, type, material);
        _GeometriesByUUID[geom->GetUUID()] = geom;
        _GeometriesByName[name] = geom;
        return geom;
    }

    bool GeometryManager::Exists(const UUID& uuid) {
        return _GeometriesByUUID.count(uuid) > 0;
    }

    Shared<Geometry> GeometryManager::Get(const UUID& uuid) {
        auto it = _GeometriesByUUID.find(uuid);
        return it != _GeometriesByUUID.end() ? it->second : nullptr;
    }

    bool GeometryManager::Exists(const std::string& name) {
        return _GeometriesByName.count(name) > 0;
    }

    Shared<Geometry> GeometryManager::Get(const std::string& name) {
        auto it = _GeometriesByName.find(name);
        return it != _GeometriesByName.end() ? it->second : nullptr;
    }

} // namespace Codi
