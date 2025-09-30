#pragma once

#include "Codi/Core/DeltaTime.h"
#include "Codi/Renderer/OrthographicCamera.h"
#include "Codi/Events/ApplicationEvent.h"
#include "Codi/Events/MouseEvent.h"

namespace Codi {

class OrthographicCameraController {
public:
    OrthographicCameraController(float aspectRatio, bool rotation = false);

    void onUpdate(DeltaTime deltatime);
    void onEvent(Event& e);

    OrthographicCamera& getCamera() { return _camera; }
    const OrthographicCamera& getCamera() const { return _camera; }

private:
    bool onMouseScrolled(MouseScrolledEvent& e);
    bool onWindowResized(WindowResizeEvent& e);

    float _aspectRatio;
    float _zoomLevel = 1.0f;

    OrthographicCamera _camera;

    bool _rotation;
    glm::vec3 _cameraPosition = { 0.0f, 0.0f, 0.0f };
    float _cameraRotation = 0.0f;
    float _cameraTranslationSpeed = 1.0f;
    float _cameraRotationSpeed = 90.0f;
};

} // namespace Codi
