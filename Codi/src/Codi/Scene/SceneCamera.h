#pragma once

#include "Codi/Renderer/Camera.h"

namespace Codi {

class SceneCamera : public Camera {
public:
    enum class ProjectionType {
        PERSPECTIVE = 0,
        ORTHOGRAPHIC = 1
    };

    SceneCamera();
    virtual ~SceneCamera() = default;

    void setViewportSize(const uint32_t width, const uint32_t height);

    /* ------------------------- PERSPECTIVE ------------------------- */

    void setPerspective(const float verticalFOV, const float nearClip, const float farClip);

    float getPerspectiveVerticalFOV() const { return _perspectiveVerticalFOV; }
    void setPerspectiveVerticalFOV(const float verticalFOV) { _perspectiveVerticalFOV = verticalFOV; _recalculateProjection(); }

    float getPerspectiveNearClip() const { return _perspectiveNearClip; }
    void setPerspectiveNearClip(const float nearClip) { _perspectiveNearClip = nearClip; _recalculateProjection(); }

    float getPerspectiveFarClip() const { return _perspectiveFarClip; }
    void setPerspectiveFarClip(const float farClip) { _perspectiveFarClip = farClip; _recalculateProjection(); }

    /* --------------------------------------------------------------- */

    /* ------------------------- ORTHOGRAPHIC ------------------------ */

    void setOrthographic(const float size, float nearClip, const float farClip);

    float getOrthographicSize() const { return _orthographicSize; }
    void setOrthographicSize(const float size) { _orthographicSize = size; _recalculateProjection(); }

    float getOrthographicNearClip() const { return _orthographicNearClip; }
    void setOrthographicNearClip(const float nearClip) { _orthographicNearClip = nearClip; _recalculateProjection(); }

    float getOrthographicFarClip() const { return _orthographicFarClip; }
    void setOrthographicFarClip(const float farClip) { _orthographicFarClip = farClip; _recalculateProjection(); }

    /* --------------------------------------------------------------- */

    ProjectionType getProjectionType() const { return _projectionType; }
    void setProjectionType(const ProjectionType type) { _projectionType = type; } 

private:
    void _recalculateProjection();
    void _recalculatePerspectiveProjection();
    void _recalculateOrthographicProjection();

    ProjectionType _projectionType = ProjectionType::ORTHOGRAPHIC;

    float _perspectiveVerticalFOV = glm::radians(45.0f);
    float _perspectiveNearClip = 0.01f;
    float _perspectiveFarClip  = 10000.0f;

    float _orthographicSize = 10.0f;
    float _orthographicNearClip = -1.0f;
    float _orthographicFarClip  =  1.0f;

    float _aspectRatio = 0.0f;
};

} // namespace Codi
