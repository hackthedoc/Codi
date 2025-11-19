#pragma once

#include <Codi.h>

namespace Codi {

    class EditorLayer : public Layer {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void OnUpdate(float deltatime) override;
    };

} // namespace Codi
