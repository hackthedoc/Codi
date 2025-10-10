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

void Scene::destroyEntity(Entity entity) {
    _registry.destroy(entity);
}

void Scene::onUpdateEditor(DeltaTime delta, EditorCamera& camera) {
    Renderer2D::BeginScene(camera);

    _registry.view<TransformComponent, SpriteRendererComponent>().each([](auto entity, TransformComponent& transform, SpriteRendererComponent& sprite) {
        Renderer2D::DrawQuad(transform.getTransform(), sprite.color);
    });

    Renderer2D::EndScene();
}

void Scene::onUpdateRuntime(DeltaTime deltatime) {
    CODI_PROFILE_FUNCTION();

    // Update Script
    {
        _registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& nsc) {
            if (!nsc.instance) {
                nsc.instance = nsc.instantiate();
                nsc.instance->_entity = Entity{ entity, this };
                
                nsc.instance->onCreate();
            }

            nsc.instance->onUpdate(deltatime);
        });
    }

    // Render 2D

    Camera* primaryCamera = nullptr;
    glm::mat4 cameraTransform; 
    {
        auto view = _registry.view<TransformComponent, CameraComponent>();
        for (const auto& [entity, transform, camera] : view.each()) {
            if (camera.primary) {
                primaryCamera = &camera.camera;
                cameraTransform = transform.getTransform();
                break;
            }
        }
    }

    if (!primaryCamera) return;

    Renderer2D::BeginScene(*primaryCamera, cameraTransform);

    _registry.view<TransformComponent, SpriteRendererComponent>().each([](auto entity, TransformComponent& transform, SpriteRendererComponent& sprite) {
        Renderer2D::DrawQuad(transform.getTransform(), sprite.color);
    });

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

Entity Scene::getPrimaryCameraEntity() {
    auto view = _registry.view<CameraComponent>();
    for (const auto& [entity, camera] : view.each())
        if (camera.primary) return Entity {entity, this};
    return Entity{};
}

template<typename T>
void Scene::onComponentAdded(Entity entity, T& component) {
    CODI_CORE_ASSERT(false);
}

template<>
void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {}

template<>
void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
    component.camera.setViewportSize(_viewportWidth, _viewprotHeight);
}

template<>
void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {}

template<>
void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component) {}

template<>
void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {}

} // namespace Codi
