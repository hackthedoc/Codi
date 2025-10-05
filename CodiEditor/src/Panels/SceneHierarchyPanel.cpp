#include "SceneHierarchyPanel.h"

#include "Codi/Scene/Component.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Codi {
    
SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) {
    setContext(scene);
}

void SceneHierarchyPanel::setContext(const Ref<Scene>& context) {
    _context = context;
}

void SceneHierarchyPanel::onImGuiRender() {
    ImGui::Begin("Scene Hierarchy");

    _context->_registry.view<TagComponent>().each([=](auto entityID, TagComponent& tc) {
        _drawEntityNode(Entity{entityID, _context.get()}, tc.tag);
    });

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) _selectionContext = {};

    ImGui::End();

    ImGui::Begin("Properties");

    if (_selectionContext) _drawComponents(_selectionContext);

    ImGui::End();
}

void SceneHierarchyPanel::_drawEntityNode(Entity entity, const std::string& tag) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (entity == _selectionContext)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

    if (ImGui::IsItemClicked()) {
        _selectionContext = entity;
    }

    if (opened) {
        ImGui::TreePop();
    }
}

void SceneHierarchyPanel::_drawComponents(Entity entity) {
    if (entity.hasComponent<TagComponent>()) {
        std::string& tag = entity.getComponent<TagComponent>().tag;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), tag.c_str());
        if (ImGui::InputText("Tag", buffer, sizeof(buffer))) tag = std::string(buffer);
    }

    if (entity.hasComponent<TransformComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
            glm::mat4& transform = entity.getComponent<TransformComponent>().transform;
            ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

            ImGui::TreePop();
        }
    }

    if (entity.hasComponent<SpriteRendererComponent>()) {
        glm::vec4& color = entity.getComponent<SpriteRendererComponent>().color;
        ImGui::ColorEdit4("Square Color", glm::value_ptr(color));
    }

    if (entity.hasComponent<CameraComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera")) {
            CameraComponent& cameraComponent = entity.getComponent<CameraComponent>();
            SceneCamera& camera = entity.getComponent<CameraComponent>().camera;

            ImGui::Checkbox("Is Primary", &cameraComponent.primary);

            const char* projectionTypeStrings[] {"Perspective", "Orthographic"};
            const char* currentProjectionString = projectionTypeStrings[(int)camera.getProjectionType()];
            if (ImGui::BeginCombo("Projection", currentProjectionString)) {
                for (int i = 0; i < 2; i++) {
                    bool isSelected = currentProjectionString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
                        currentProjectionString = projectionTypeStrings[i];
                        camera.setProjectionType((SceneCamera::ProjectionType)i);
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            if (camera.getProjectionType() == SceneCamera::ProjectionType::PERSPECTIVE) {
                float verticalFOV = glm::degrees(camera.getPerspectiveVerticalFOV());
                if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) { camera.setPerspectiveVerticalFOV(glm::radians(verticalFOV)); }

                float nearClip = camera.getPerspectiveNearClip();
                if (ImGui::DragFloat("Near", &nearClip)) { camera.setPerspectiveNearClip(nearClip); }

                float farClip = camera.getPerspectiveFarClip();
                if (ImGui::DragFloat("Far", &farClip)) { camera.setPerspectiveFarClip(farClip); }
            }

            if (camera.getProjectionType() == SceneCamera::ProjectionType::ORTHOGRAPHIC) {
                float size = camera.getOrthographicSize();
                if (ImGui::DragFloat("Size", &size)) { camera.setOrthographicSize(size); }

                float nearClip = camera.getOrthographicNearClip();
                if (ImGui::DragFloat("Near", &nearClip)) { camera.setOrthographicNearClip(nearClip); }

                float farClip = camera.getOrthographicFarClip();
                if (ImGui::DragFloat("Far", &farClip)) { camera.setOrthographicFarClip(farClip); }

                ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.fixedAspectRatio);
            }

            ImGui::TreePop();
        }
    }
}

} // namespace Codi

