#pragma once

#include "Scene.h"

#include "entt.hpp"

namespace Codi {
    
class Entity {
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene);
    Entity(const Entity& other) = default;

    template<typename T>
    bool hasComponent() {
        return _scene->_registry.any_of<T>(_entityHandle);
    }

    template<typename T>
    T& getComponent() { 
        CODI_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!"); 
        return _scene->_registry.get<T>(_entityHandle); 
    }

    template<typename T, typename ... Args>
    T& addComponent(Args&&... args) { 
        CODI_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!"); 
        return _scene->_registry.emplace<T>(_entityHandle, std::forward<Args>(args)...); 
    }

    template<typename T>
    void removeComponent() {
        CODI_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!"); 
        _scene->_registry.remove<T>(_entityHandle);
    }

    operator bool() const { return _entityHandle != entt::null; }

private:
    entt::entity _entityHandle{ entt::null };
    Scene* _scene{ nullptr };
};

} // namespace Codi
