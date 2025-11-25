#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/Material.h"

#include <unordered_map>

namespace Codi {

    class MaterialManager {
    public:
        static void Init();
        static void Shutdown();

        static Shared<MaterialStandard> Create(const std::string& name);

        static bool Exists(const UUID& uuid);
        static Shared<Material> Get(const UUID& uuid);

        static bool Exists(const std::string& path);
        static Shared<Material> Get(const std::string& name);

    private:
        MaterialManager() = default;

        static std::unordered_map<UUID, Shared<Material>> _MaterialsByUUID;
        static std::unordered_map<std::string, Shared<Material>> _MaterialsByName;
    };

} // namespace Codi
