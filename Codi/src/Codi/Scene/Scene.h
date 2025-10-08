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
    void destroyEntity(Entity entity);

    void onUpdate(DeltaTime deltatime);
    void onViewportResize(uint32_t width, uint32_t height);

    Entity getPrimaryCameraEntity();

private:
    template<typename T>
    void onComponentAdded(Entity entity, T& component);

    entt::registry _registry;
    uint32_t _viewportWidth = 0;
    uint32_t _viewprotHeight = 0;

    friend Entity;
    friend class SceneHierarchyPanel;
    friend class SceneSerializer;
};
    
} // namespace Codi
