#pragma once

#include <Codi.h>

class Sandbox2D : public Codi::Layer {
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    virtual void onAttach() override;
    virtual void onDetach() override;

    void onUpdate(Codi::DeltaTime deltatime) override;
    virtual void onImGuiRender() override;
    void onEvent(Codi::Event& e) override;

private:
    Codi::OrthographicCameraController _cameraController;

    // TEMP
    Codi::Ref<Codi::Texture2D> _texture;
    glm::vec4 _squareColor = { 1.0f, 0.0f, 0.0f, 1.0f };
    Codi::Ref<Codi::FrameBuffer> _frameBuffer;
};
