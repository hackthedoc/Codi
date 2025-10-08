#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Codi/Scene/SceneCamera.h"
#include "Codi/Scene/ScriptableEntity.h"

namespace Codi {

struct TagComponent {
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& t) : tag(t) {}
};

struct TransformComponent {
    glm::vec3 translation   = glm::vec3(0.0f);
    glm::vec3 rotation      = glm::vec3(0.0f);
    glm::vec3 scale         = glm::vec3(1.0f);

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& t) : translation(t) {}

    glm::mat4 getTransform() const {
        glm::mat4 rot = glm::toMat4(glm::quat(rotation));
        glm::mat4 scl = glm::scale(glm::mat4(1.0f), scale);
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
        return trans * rot * scl;
    }
};

struct SpriteRendererComponent {
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const SpriteRendererComponent&) = default;
    SpriteRendererComponent(const glm::vec4& c) : color(c) {}

    operator glm::vec4& () { return color; }
    operator const glm::vec4& () const { return color; }
};

struct CameraComponent {
    SceneCamera camera;
    bool primary = true;
    bool fixedAspectRatio = false;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
};

struct NativeScriptComponent {
    ScriptableEntity* instance = nullptr;

    ScriptableEntity*(*instantiate)();
    void(*destroyInstance)(NativeScriptComponent*);

    template<typename T>
    void bind() {
        instantiate = []() { return static_cast<ScriptableEntity*>(new T()); };
        destroyInstance = [](NativeScriptComponent* nsc) { delete nsc->instance; nsc->instance = nullptr; };
    }
};

} // namespace Codi
