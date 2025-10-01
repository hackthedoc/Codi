#include <Codi.h>
#include <Codi/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Codi {

class CodiEditorApp: public Application {
public:
    CodiEditorApp() : Application("Codi Editor") {
        pushLayer(new EditorLayer());
    }
    ~CodiEditorApp() {}
};

Application* CreateApplication() {
    return new CodiEditorApp();
}

} // namespace Codi

