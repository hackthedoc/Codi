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
    bool hasComponent() const noexcept {
        return _scene->_registry.all_of<T>(_entityHandle);
    }

    template<typename T>
    T& getComponent() { 
        CODI_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!"); 
        return _scene->_registry.get<T>(_entityHandle); 
    }

    template<typename T, typename ... Args>
    T& addComponent(Args&&... args) { 
        CODI_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!"); 
        T& component = _scene->_registry.emplace<T>(_entityHandle, std::forward<Args>(args)...); 
        _scene->onComponentAdded<T>(*this, component);
        return component;
    }

    template<typename T, typename ... Args>
    T& getOrCreateComponent(Args&&... args) { 
        if (hasComponent<T>()) return getComponent<T>();
        else return addComponent<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    void removeComponent() {
        CODI_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!"); 
        _scene->_registry.remove<T>(_entityHandle);
    }

    operator bool() const { return _entityHandle != entt::null; }
    operator entt::entity() const { return _entityHandle; }
    operator uint32_t() const { return (uint32_t)_entityHandle; }

    bool operator==(const Entity& o) const { return _entityHandle == o._entityHandle && _scene == o._scene; }
    bool operator!=(const Entity& o) const { return _entityHandle != o._entityHandle || _scene != o._scene; }

private:
    entt::entity _entityHandle{ entt::null };
    Scene* _scene{ nullptr };
};

} // namespace Codi
