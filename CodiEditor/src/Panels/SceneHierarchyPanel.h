#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Scene/Scene.h"
#include "Codi/Scene/Entity.h"

namespace Codi {

class SceneHierarchyPanel {
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene>& scene);

    void setContext(const Ref<Scene>& context);

    void onImGuiRender();

private:
    void _drawEntityNode(Entity entity, const std::string& tag);
    void _drawComponents(Entity entity);

    Ref<Scene> _context;
    Entity _selectionContext;
};

} // namespace Codi
