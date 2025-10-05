#include "cdpch.h"
#include "Entity.h"

namespace Codi {

Entity::Entity(entt::entity handle, Scene* scene)
    : _entityHandle(handle)
    , _scene(scene)
    {}

} // namespace Codi