#include "cdpch.h"
#include "OrthographicCameraController.h"

#include "Codi/Core/Input.h"
#include "Codi/Core/KeyCodes.h"

namespace Codi {

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
    : _aspectRatio(aspectRatio), _rotation(rotation),
      _camera(-_aspectRatio * _zoomLevel, _aspectRatio * _zoomLevel, -_zoomLevel, _zoomLevel)
    {}

void OrthographicCameraController::onUpdate(DeltaTime deltatime) {
    CODI_PROFILE_FUNCTION();
    
    _cameraTranslationSpeed = _zoomLevel;

    if (Input::IsKeyPressed(KeyCode::W))
        _cameraPosition.y += _cameraTranslationSpeed * deltatime;
    if (Input::IsKeyPressed(KeyCode::A))
        _cameraPosition.x -= _cameraTranslationSpeed * deltatime;
    if (Input::IsKeyPressed(KeyCode::S))
        _cameraPosition.y -= _cameraTranslationSpeed * deltatime;
    if (Input::IsKeyPressed(KeyCode::D))
        _cameraPosition.x += _cameraTranslationSpeed * deltatime;

    _camera.setPosition(_cameraPosition);

    if (!_rotation) return;

    if (Input::IsKeyPressed(KeyCode::Q))
        _cameraRotation -= _cameraRotationSpeed * deltatime;
    if (Input::IsKeyPressed(KeyCode::E))
        _cameraRotation += _cameraRotationSpeed * deltatime;
            
    _camera.setRotation(_cameraRotation);    
}

void OrthographicCameraController::onEvent(Event& e) {
    CODI_PROFILE_FUNCTION();
    
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<MouseScrolledEvent>(CODI_BIND_EVENT_FN(OrthographicCameraController::onMouseScrolled));
    dispatcher.dispatch<WindowResizeEvent>(CODI_BIND_EVENT_FN(OrthographicCameraController::onWindowResized));
}

void OrthographicCameraController::onResize(const float width, const float height) {
    CODI_PROFILE_FUNCTION();
    
    _aspectRatio = width / height;
    _camera.setProjection(-_aspectRatio * _zoomLevel, _aspectRatio * _zoomLevel, -_zoomLevel, _zoomLevel);
}

bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& e) {
    CODI_PROFILE_FUNCTION();
    
    _zoomLevel -= e.getYOffset() * 0.25f;
    _zoomLevel = std::max(_zoomLevel, 0.25f);
    _camera.setProjection(-_aspectRatio * _zoomLevel, _aspectRatio * _zoomLevel, -_zoomLevel, _zoomLevel);
    return false;
}

bool OrthographicCameraController::onWindowResized(WindowResizeEvent& e) {
    CODI_PROFILE_FUNCTION();
    
    onResize((float)e.getWidth(), (float)e.getHeight());
    return false;
}

} // namespace Codi
