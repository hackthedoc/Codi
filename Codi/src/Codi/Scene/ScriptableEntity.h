#pragma once

#include "Codi/Core/DeltaTime.h"

#include "Codi/Scene/Entity.h"

namespace Codi {

class ScriptableEntity {
public:
    virtual ~ScriptableEntity() {};

    template<typename T>
    T& getComponent() { return _entity.getComponent<T>(); }

protected:
    virtual void onCreate() {}
    virtual void onDestroy() {}
    virtual void onUpdate(DeltaTime deltatime) {}

private:
    Entity _entity;
    friend class Scene;
};

} // namespace Codi

