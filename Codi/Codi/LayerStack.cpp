#include "LayerStack.h"

namespace Codi {

LayerStack::LayerStack() {
    _layerInsert = begin();
}

LayerStack::~LayerStack() {
    for (Layer* layer : _layers)
        delete layer;
}

void LayerStack::pushLayer(Layer* layer) {
    _layerInsert = _layers.emplace(_layerInsert, layer);
}

void LayerStack::pushOverlay(Layer* overlay) {
    _layers.emplace_back(overlay);
}

void LayerStack::popLayer(Layer* layer) {
    std::vector<Layer*>::iterator it = std::find(begin(), end(), layer);
    if (it == end()) return;
    _layers.erase(it);
    _layerInsert--;    
}

void LayerStack::popOverlay(Layer* overlay) {
    std::vector<Layer*>::iterator it = std::find(begin(), end(), overlay);
    if (it == end()) return;
    _layers.erase(it);
}

}
