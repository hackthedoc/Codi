#include "codipch.h"
#include "MaterialManager.h"

namespace Codi {

    std::unordered_map<UUID, Shared<Material>> MaterialManager::_MaterialsByUUID;
    std::unordered_map<std::string, Shared<Material>> MaterialManager::_MaterialsByName;

    void MaterialManager::Init() {
        _MaterialsByUUID.clear();
        _MaterialsByName.clear();

        CODI_CORE_INFO("Material Manager initialized!");
    }

    void MaterialManager::Shutdown() {
        CODI_CORE_INFO("Cleaning Material Manager...");

        for (auto& [uuid, material] : _MaterialsByUUID)
            material->Destroy();

        _MaterialsByUUID.clear();
        _MaterialsByName.clear();
    }

    Shared<MaterialStandard> MaterialManager::Create(const std::string& name) {
        // Avoid duplicates by name
        auto it = _MaterialsByName.find(name);
        if (it != _MaterialsByName.end())
            return std::dynamic_pointer_cast<MaterialStandard>(it->second);

        Shared<MaterialStandard> material = MaterialStandard::Create(name);
        _MaterialsByUUID[material->GetUUID()] = material;
        _MaterialsByName[name] = material;
        return material;
    }

    // UUID lookup
    bool MaterialManager::Exists(const UUID& uuid) {
        return _MaterialsByUUID.count(uuid) > 0;
    }

    Shared<Material> MaterialManager::Get(const UUID& uuid) {
        auto it = _MaterialsByUUID.find(uuid);
        return it != _MaterialsByUUID.end() ? it->second : nullptr;
    }

    // Name lookup
    bool MaterialManager::Exists(const std::string& name) {
        return _MaterialsByName.count(name) > 0;
    }

    Shared<Material> MaterialManager::Get(const std::string& name) {
        auto it = _MaterialsByName.find(name);
        return it != _MaterialsByName.end() ? it->second : nullptr;
    }

} // namespace Codi
