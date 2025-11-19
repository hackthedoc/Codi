#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Core/Layer.h"

namespace Codi {

    class LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);

        std::vector<Layer*>::iterator begin() { return _Layers.begin(); }
        std::vector<Layer*>::iterator end() { return _Layers.end(); }

    private:
        std::vector<Layer*> _Layers;
        uint32 _LayerIndex = 0;
    };

} // namespace Codi#pragma once
