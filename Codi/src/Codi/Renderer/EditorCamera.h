#pragma once

#include "Codi/Core/DeltaTime.h"
#include "Codi/Events/Event.h"
#include "Codi/Events/MouseEvent.h"
#include "Codi/Renderer/Camera.h"

#include <glm/glm.hpp>

namespace Codi {

class EditorCamera : public Camera {
public:
    EditorCamera() = default;
    EditorCamera(const float fov, const float aspectRatio, const float nearClip, const float farClip);

    void onUpdate(const DeltaTime dt);
    void onEvent(Event& e);

    inline float getDistance() const { return _distance; }
    inline void setDistance(const float d) { _distance = d; }

    inline void setViewportSize(const float w, const float h) { _viewportWidth = w; _viewportHeight = h; _updateProjection(); }

    const glm::mat4& getViewMatrix() const { return _viewMatrix; }
    glm::mat4 getViewProjection() const { return _projection * _viewMatrix; }

    glm::vec3 getUpDirection() const;
    glm::vec3 getDownDirection() const;
    glm::vec3 getRightDirection() const;
    glm::vec3 getLeftDirection() const;
    glm::vec3 getForwardDirection() const;
    const glm::vec3& getDirection() const { return _position; }
    glm::quat getOrientation() const;

    float getPitch() const { return _pitch; }
    float getYaw() const { return _yaw; }

private:
    float _FOV = 45.0f;
    float _aspectRatio = 1.778f;
    float _nearClip = 0.1f;
    float _farClip = 1000.0f;

    glm::mat4 _viewMatrix;
    glm::vec3 _position = { 0.0f, 0.0f, 0.0f};
    glm::vec3 _focalPoint = { 0.0f, 0.0f, 0.0f};

    glm::vec2 _initialMousePosition = { 0.0f, 0.0f };

    float _distance = 10.0f;
    float _pitch = 0.0f;
    float _yaw = 0.0f;

    float _viewportWidth = 1280.0f;
    float _viewportHeight = 720.0f;

    void _updateProjection();
    void _updateView();

    bool _onMouseScroll(MouseScrolledEvent& e);

    void _mousePan(const glm::vec2& delta);
    void _mouseRotate(const glm::vec2& delta);
    void _mouseZoom(const float delta);

    glm::vec3 _calculatePosition() const;

    std::pair<float, float> _panSpeed() const;
    float _rotationSpeed() const;
    float _zoomSpeed() const;
};

} // namespace Codi
