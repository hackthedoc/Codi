#include "codipch.h"
#include "LayerStack.h"

namespace Codi {

    LayerStack::LayerStack() {
        _LayerIndex = 0;
    }

    LayerStack::~LayerStack() {
        for (Layer* layer : _Layers)
            delete layer;
    }

    void LayerStack::PushLayer(Layer* layer) {
        _Layers.emplace(begin() + _LayerIndex, layer);
        _LayerIndex++;
    }

    void LayerStack::PushOverlay(Layer* overlay) {
        _Layers.emplace_back(overlay);
    }

    void LayerStack::PopLayer(Layer* layer) {
        std::vector<Layer*>::iterator it = std::find(begin(), end(), layer);
        if (it == end()) return;
        _Layers.erase(it);
        _LayerIndex--;
    }

    void LayerStack::PopOverlay(Layer* overlay) {
        std::vector<Layer*>::iterator it = std::find(begin(), end(), overlay);
        if (it == end()) return;
        _Layers.erase(it);
    }

} // namespace Codi
