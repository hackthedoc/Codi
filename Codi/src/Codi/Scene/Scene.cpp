#include "cdpch.h"
#include "Scene.h"

#include "Codi/Scene/Entity.h"
#include "Codi/Scene/Component.h"
#include "Codi/Renderer/Renderer2D.h"

namespace Codi {

Scene::Scene() {}

Scene::~Scene() {}

Entity Scene::createEntity(const std::string& name) {
    Entity entity{_registry.create(), this};
    
    TagComponent tag = entity.addComponent<TagComponent>(name);
    if (tag.tag.empty()) tag.tag = "Entity";
    
    entity.addComponent<TransformComponent>();
    
    return entity; 
}

void Scene::onUpdate(DeltaTime deltatime) {
    auto group = _registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group) {
        const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

        Renderer2D::DrawQuad(transform, sprite.color);
    }
}

} // namespace Codi
