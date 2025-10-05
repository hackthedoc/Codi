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
}

} // namespace Codi

