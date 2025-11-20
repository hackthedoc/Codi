#pragma once

#include "Codi/Events/Event.h"
#include "Codi/Events/MouseEvents.h"
#include "Codi/Renderer/Camera.h"

#include <glm/glm.hpp>

namespace Codi {

    class EditorCamera : public Camera {
    public:
        EditorCamera() = default;
        EditorCamera(const float32 fov, const float32 aspectRatio, const float32 nearClip, const float32 farClip);

        void OnUpdate(const float32 deltatime);
        void OnEvent(Event& e);

        inline float32 GetDistance() const { return _Distance; }
        inline void SetDistance(const float distance) { _Distance = distance; }

        inline void SetViewportSize(const float32 w, const float32 h) { _ViewportWidth = w; _ViewportHeight = h; UpdateProjection(); }

        const glm::mat4& GetViewMatrix() const { return _ViewMatrix; }
        glm::mat4 GetViewProjection() const { return _Projection * _ViewMatrix; }

        glm::vec3 GetUpDirection() const;
        glm::vec3 GetDownDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetLeftDirection() const;
        glm::vec3 GetForwardDirection() const;
        const glm::vec3& GetDirection() const { return _Position; }
        glm::quat GetOrientation() const;

        float32 GetPitch() const { return _Pitch; }
        float32 GetYaw() const { return _Yaw; }

    private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent& e);

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(const float32 delta);

        glm::vec3 CalculatePosition() const;

        std::pair<float32, float32> PanSpeed() const;
        float32 RotationSpeed() const;
        float32 ZoomSpeed() const;

    private:
        float32 _FOV = 45.0f;
        float32 _AspectRatio = 1.778f;
        float32 _NearClip = 0.1f;
        float32 _FarClip = 1000.0f;

        glm::mat4 _ViewMatrix = glm::mat4(0.0f);;
        glm::vec3 _Position = glm::vec3(0.0f);
        glm::vec3 _FocalPoint = glm::vec3(0.0f);

        glm::vec2 _InitialMousePosition = glm::vec2(0.0f);

        float32 _Distance = 10.0f;
        float32 _Pitch = 0.0f;
        float32 _Yaw = 0.0f;

        float32 _ViewportWidth = 1280.0f;
        float32 _ViewportHeight = 720.0f;
    };

} // namespace Codi
