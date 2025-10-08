#pragma once

#include "Codi/Scene/Scene.h"

namespace Codi {
    
class SceneSerializer {
public:
    SceneSerializer(const Ref<Scene>& scene);
    ~SceneSerializer();

    void serializeText(const std::string& filepath);
    void serializeBinary(const std::string& filepath);

    bool deserializeText(const std::string& filepath);
    bool deserializeBinary(const std::string& filepath);

private:
    Ref<Scene> _scene;
};

} // namespace Codi
