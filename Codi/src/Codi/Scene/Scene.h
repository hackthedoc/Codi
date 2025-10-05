#pragma once

#include "Codi/Core/DeltaTime.h"

#include "entt.hpp"

namespace Codi {

class Entity;

class Scene {
public:
    Scene();
    ~Scene();

    Entity createEntity(const std::string& name = std::string());

    void onUpdate(DeltaTime deltatime);
private:
    entt::registry _registry;

    friend Entity;
};
    
} // namespace Codi
