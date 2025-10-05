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
    CODI_PROFILE_FUNCTION();

    Camera* primaryCamera = nullptr;
    glm::mat4* sceneTransform = nullptr; 
    {
        auto view = _registry.view<TransformComponent, CameraComponent>();
        for (const auto& [entity, transform, camera] : view.each()) {
            if (camera.primary) {
                primaryCamera = &camera.camera;
                sceneTransform = &transform.transform;
                break;
            }
        }
    }

    if (!primaryCamera) return;

    Renderer2D::BeginScene(*primaryCamera, *sceneTransform);

    auto view = _registry.view<TransformComponent, SpriteRendererComponent>();
    for (const auto& [entity, transform, sprite] : view.each())
        Renderer2D::DrawQuad(transform, sprite.color);

    Renderer2D::EndScene();
}

void Scene::onViewportResize(uint32_t width, uint32_t height) {
    _viewportWidth = width;
    _viewprotHeight = height;

    auto view = _registry.view<CameraComponent>();
    for (const auto& [entity, camera] : view.each()) {
        if (!camera.fixedAspectRatio) {
            camera.camera.setViewportSize(width, height);
        }
    }
}

} // namespace Codi
