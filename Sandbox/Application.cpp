#include <Codi.h>

class ExampleLayer: public Codi::Layer {
public:
    ExampleLayer() : Layer("Example") {}

    void onUpdate() override { CINFO("ExampleLayer::Update"); }
    void onEvent(Codi::Event& e) override { CTRACE("{0}", e.toString()); }
};

class Sandbox: public Codi::Application {
public:
    Sandbox() {
        pushLayer(new ExampleLayer());
        pushOverlay(new Codi::ImGuiLayer());
    }
    ~Sandbox() {}
};

Codi::Application* Codi::CreateApplication() {
    return new Sandbox();
}
