#include "codipch.h"
#include "EditorCamera.h"

#include "Codi/Input/Input.h"
#include "Codi/Input/KeyCode.h"
#include "Codi/Input/MouseCode.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Codi {

    EditorCamera::EditorCamera(const float32 fov, const float32 aspectRatio, const float32 nearClip, const float32 farClip)
        : _FOV(fov)
        , _AspectRatio(aspectRatio)
        , _NearClip(nearClip)
        , _FarClip(farClip)
        , Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
    {
        UpdateView();
    }

    void EditorCamera::OnUpdate(const float32 dt) {
        if (Input::IsKeyPressed(KeyCode::LEFT_ALT)) {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
            const glm::vec2 delta = (mouse - _InitialMousePosition) * 0.003f;
            _InitialMousePosition = mouse;

            if (Input::IsMouseButtonPressed(MouseCode::BUTTON_MIDDLE))
                MousePan(delta);
            else if (Input::IsMouseButtonPressed(MouseCode::BUTTON_LEFT))
                MouseRotate(delta);
            else if (Input::IsMouseButtonPressed(MouseCode::BUTTON_RIGHT))
                MouseZoom(delta.y);
        }

        UpdateView();
    }

    void EditorCamera::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(CODI_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
    }

    glm::vec3 EditorCamera::GetUpDirection() const {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetDownDirection() const {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, -1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetRightDirection() const {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetLeftDirection() const {
        return glm::rotate(GetOrientation(), glm::vec3(-1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetForwardDirection() const {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::quat EditorCamera::GetOrientation() const {
        return glm::quat(glm::vec3(-_Pitch, -_Yaw, 0.0f));
    }

    void EditorCamera::UpdateProjection() {
        _AspectRatio = _ViewportWidth / _ViewportHeight;
        _Projection = glm::perspective(glm::radians(_FOV), _AspectRatio, _NearClip, _FarClip);
    }

    void EditorCamera::UpdateView() {
        // _Yaw = _Pitch = 0.0f; // lock the camera's rotation
        _Position = CalculatePosition();

        glm::quat orientation = GetOrientation();
        _ViewMatrix = glm::translate(glm::mat4(1.0f), _Position) * glm::toMat4(orientation);
        _ViewMatrix = glm::inverse(_ViewMatrix);
    }

    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e) {
        const float32 delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();

        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta) {
        auto [xSpeed, ySpeed] = PanSpeed();
        _FocalPoint += GetLeftDirection() * delta.x * xSpeed * _Distance;
        _FocalPoint += GetUpDirection() * delta.y * ySpeed * _Distance;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta) {
        const float32 yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        _Yaw += yawSign * delta.x * RotationSpeed();
        _Pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(const float32 delta) {
        _Distance -= delta * ZoomSpeed();

        if (_Distance >= 1.0f) return;

        _FocalPoint += GetForwardDirection();
        _Distance = 1.0f;
    }

    glm::vec3 EditorCamera::CalculatePosition() const {
        return _FocalPoint - GetForwardDirection() * _Distance;
    }

    std::pair<float32, float32> EditorCamera::PanSpeed() const {
        float32 x = std::min(_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float32 xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float32 y = std::min(_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float32 yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float32 EditorCamera::RotationSpeed() const {
        return 0.8f;
    }

    float32 EditorCamera::ZoomSpeed() const {
        float32 distance = _Distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float32 speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }

} // namespace Codi
