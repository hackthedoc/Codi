#include "SceneHierarchyPanel.h"

#include "Codi/Scene/Component.h"

#include <imgui.h>
#include <imgui_internal.h>

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

    // right click popup in blank space
    if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Create Empty Entity")) _context->createEntity("Empty Entity");

        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Properties");

    if (_selectionContext) {
        _drawComponents(_selectionContext);

        if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent")) {
            if (ImGui::MenuItem("Camera")) {
                _selectionContext.addComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Sprite Renderer")) {
                _selectionContext.addComponent<SpriteRendererComponent>();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

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

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) entityDeleted = true;

        ImGui::EndPopup();
    }

    if (opened) {
        ImGui::TreePop();
    }

    if (entityDeleted) {
        if (entity == _selectionContext) _selectionContext = {};

        _context->destroyEntity(entity);
    }
}

static void DrawVec3Control(const std::string& label, glm::vec3& vec, const float resetValue = 0.0f, const float columnWidth = 100.0f) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0,0});

    float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.25f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f});
    if (ImGui::Button("X", buttonSize)) vec.x = resetValue;
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &vec.x, 0.1f);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f});
    if (ImGui::Button("Y", buttonSize)) vec.y = resetValue;
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &vec.y, 0.1f);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f});
    if (ImGui::Button("Z", buttonSize)) vec.z = resetValue; 
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &vec.z, 0.1f);
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

void SceneHierarchyPanel::_drawComponents(Entity entity) {
    if (entity.hasComponent<TagComponent>()) {
        std::string& tag = entity.getComponent<TagComponent>().tag;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), tag.c_str());
        if (ImGui::InputText("Tag", buffer, sizeof(buffer))) tag = std::string(buffer);
    }

    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

    if (entity.hasComponent<TransformComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform")) {
            TransformComponent& tc = entity.getComponent<TransformComponent>();
            DrawVec3Control("Translation", tc.translation);
            glm::vec3 rotation = glm::degrees(tc.rotation);
            DrawVec3Control("Rotation", rotation);
            tc.rotation = glm::radians(rotation);
            DrawVec3Control("Scale", tc.scale, 1.0f);

            ImGui::TreePop();
        }
    }

    if (entity.hasComponent<SpriteRendererComponent>()) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Texture");
        ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
        if (ImGui::Button("+", ImVec2{ 20, 20 })) ImGui::OpenPopup("ComponentSettings");
        
        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("Remove")) removeComponent = true;
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        if (open) {
            glm::vec4& color = entity.getComponent<SpriteRendererComponent>().color;
            ImGui::ColorEdit4("Square Color", glm::value_ptr(color));

            ImGui::TreePop();
        }

        if (removeComponent) entity.removeComponent<SpriteRendererComponent>();
    }

    if (entity.hasComponent<CameraComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), treeNodeFlags, "Camera")) {
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

