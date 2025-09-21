#include "Codi.h"

class Sandbox: public Codi::Application {
public:
    Sandbox() {}
    ~Sandbox() {}
};

Codi::Application* Codi::CreateApplication() {
    return new Sandbox();
}
