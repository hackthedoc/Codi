#include <Codi.h>
#include <Codi/Core/EntryPoint.h>

#include "Sandbox2D.h"

class Sandbox: public Codi::Application {
public:
    Sandbox() : Codi::Application("Sandbox") {
        pushLayer(new Sandbox2D());
    }
    ~Sandbox() {}
};

Codi::Application* Codi::CreateApplication() {
    return new Sandbox();
}
