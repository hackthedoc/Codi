#include <Codi.h>

class ExampleLayer: public Codi::Layer {
public:
    ExampleLayer() : Layer("Example") {}

    void onUpdate() override {
        if (Codi::Input::IsKeyPressed(CODI_KEY_TAB))
            CTRACE("Tab key is pressed!");
    }
    void onEvent(Codi::Event& e) override { 
        //CTRACE("{0}", e.toString()); 
    }
};

class Sandbox: public Codi::Application {
public:
    Sandbox() {
        pushLayer(new ExampleLayer());
    }
    ~Sandbox() {}
};

Codi::Application* Codi::CreateApplication() {
    return new Sandbox();
}
