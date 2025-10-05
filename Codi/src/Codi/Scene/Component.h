#pragma once

#include <glm/glm.hpp>

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
    glm::mat4 transform{1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::mat4& t) : transform(t) {}

    operator glm::mat4& () { return transform; }
    operator const glm::mat4& () const { return transform; }
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
