#include "codipch.h"
#include "Material.h"

namespace Codi {

    MaterialStandard::MaterialStandard(const std::string& name) : _Name(name) {}

    void MaterialStandard::Destroy() {
        _Albedo.reset();
    }

    Shared<MaterialStandard> MaterialStandard::Create(const std::string& name) {
        return Share<MaterialStandard>(name);
    }

} // namespace Codi
