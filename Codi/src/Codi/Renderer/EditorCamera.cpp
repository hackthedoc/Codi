#include "cdpch.h"
#include "EditorCamera.h"

#include "Codi/Core/Input.h"
#include "Codi/Core/KeyCodes.h"
#include "Codi/Core/MouseButtonCodes.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Codi {

EditorCamera::EditorCamera(const float fov, const float aspectRatio, const float nearClip, const float farClip)
    : _FOV(fov)
    , _aspectRatio(aspectRatio)
    , _nearClip(nearClip)
    , _farClip(farClip)
    , Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
    {
    _updateView();
}

void EditorCamera::onUpdate(const DeltaTime dt) {
    if (Input::IsKeyPressed(KeyCode::LEFT_ALT)) {
        const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
        const glm::vec2 delta = (mouse - _initialMousePosition) * 0.003f;
        _initialMousePosition = mouse;

        if (Input::IsMouseButtonPressed(MouseCode::BUTTON_MIDDLE))
            _mousePan(delta);
        else if (Input::IsMouseButtonPressed(MouseCode::BUTTON_LEFT))
            _mouseRotate(delta);
        else if (Input::IsMouseButtonPressed(MouseCode::BUTTON_RIGHT))
            _mouseZoom(delta.y);
    }

    _updateView();
}

void EditorCamera::onEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<MouseScrolledEvent>(CODI_BIND_EVENT_FN(EditorCamera::_onMouseScroll));
}

glm::vec3 EditorCamera::getUpDirection() const {
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 EditorCamera::getDownDirection() const {
    return glm::rotate(getOrientation(), glm::vec3(0.0f, -1.0f, 0.0f));
}

glm::vec3 EditorCamera::getRightDirection() const {
    return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 EditorCamera::getLeftDirection() const {
    return glm::rotate(getOrientation(), glm::vec3(-1.0f, 0.0f, 0.0f));
}

glm::vec3 EditorCamera::getForwardDirection() const {
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::quat EditorCamera::getOrientation() const {
    return glm::quat(glm::vec3(-_pitch, -_yaw, 0.0f));
}

void EditorCamera::_updateProjection() {
    _aspectRatio = _viewportWidth / _viewportHeight;
    _projection = glm::perspective(glm::radians(_FOV), _aspectRatio, _nearClip, _farClip);
}

void EditorCamera::_updateView() {
    // _yaw = _pitch = 0.0f; // lock the camera's rotation
    _position = _calculatePosition();

    glm::quat orientation = getOrientation();
    _viewMatrix = glm::translate(glm::mat4(1.0f), _position) * glm::toMat4(orientation);
    _viewMatrix = glm::inverse(_viewMatrix);
}

bool EditorCamera::_onMouseScroll(MouseScrolledEvent& e) {
    const float delta = e.getYOffset() * 0.1f;
    _mouseZoom(delta);
    _updateView();

    return false;
}

void EditorCamera::_mousePan(const glm::vec2& delta) {
    auto [xSpeed, ySpeed] = _panSpeed();
    _focalPoint += getLeftDirection() * delta.x * xSpeed * _distance;
    _focalPoint += getUpDirection() * delta.y * ySpeed * _distance; 
}

void EditorCamera::_mouseRotate(const glm::vec2& delta) {
    const float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
    _yaw += yawSign * delta.x * _rotationSpeed();
    _pitch += delta.y * _rotationSpeed();
}

void EditorCamera::_mouseZoom(const float delta) {
    _distance -= delta* _zoomSpeed();
    
    if (_distance >= 1.0f) return;

    _focalPoint += getForwardDirection();
    _distance = 1.0f;
}

glm::vec3 EditorCamera::_calculatePosition() const {
    return _focalPoint - getForwardDirection() * _distance;
}

std::pair<float, float> EditorCamera::_panSpeed() const {
    float x = std::min(_viewportWidth / 1000.0f, 2.4f); // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(_viewportHeight / 1000.0f, 2.4f); // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return { xFactor, yFactor };
}

float EditorCamera::_rotationSpeed() const {
    return 0.8f;
}

float EditorCamera::_zoomSpeed() const {
    float distance = _distance * 0.2f;
    distance = std::max(distance, 0.0f);
    float speed = distance * distance;
    speed = std::min(speed, 100.0f); // max speed = 100
    return speed;
}

};
