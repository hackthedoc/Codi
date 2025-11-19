#pragma once

#include <Codi.h>

namespace Codi {

    class EditorLayer : public Layer {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate(float32 deltatime) override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event& e) override;

    private:
        bool OnKeyPressed(const KeyPressedEvent& e);
        bool OnMouseButtonPressed(const MouseButtonPressedEvent& e);

    private:
        bool _ViewportFocused = false;
        bool _ViewportHovered = false;
        glm::vec2 _ViewportSize = glm::vec2(0.0f);
        glm::vec2 _ViewportBounds[2];
    };

} // namespace Codi
