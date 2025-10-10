#include "cdpch.h"
#include "SceneCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Codi {

SceneCamera::SceneCamera() {
    _recalculateProjection();
}

void SceneCamera::setPerspective(const float verticalFOV, const float nearClip, const float farClip) {
    _projectionType = ProjectionType::PERSPECTIVE;

    _perspectiveVerticalFOV = verticalFOV;
    _perspectiveNearClip = nearClip;
    _perspectiveFarClip  = farClip;

    _recalculateProjection();
}

void SceneCamera::setOrthographic(const float size, const float nearClip, const float farClip) {
    _projectionType = ProjectionType::ORTHOGRAPHIC;

    _orthographicSize = size;
    _orthographicNearClip = nearClip;
    _orthographicFarClip  = farClip;

    _recalculateProjection();
}

void SceneCamera::setViewportSize(const uint32_t width, const uint32_t height) {
    if (height == 0) _aspectRatio = (float)width;
    else _aspectRatio = (float)width / (float)height;
    _recalculateProjection();
}

void SceneCamera::_recalculateProjection() {
    if (_projectionType == ProjectionType::PERSPECTIVE) _recalculatePerspectiveProjection();
    else _recalculateOrthographicProjection();
}

void SceneCamera::_recalculatePerspectiveProjection() {
    _projection = glm::perspectiveRH_ZO(_perspectiveVerticalFOV, _aspectRatio, _perspectiveNearClip, _perspectiveFarClip);
}

void SceneCamera::_recalculateOrthographicProjection() {
    const float orthoLeft   = -_orthographicSize * 0.5f * _aspectRatio;
    const float orthoRight  =  _orthographicSize * 0.5f * _aspectRatio;
    const float orthoBottom = -_orthographicSize * 0.5f;
    const float orthoTop    =  _orthographicSize * 0.5f;

    _projection = glm::orthoRH_ZO(orthoLeft, orthoRight, orthoBottom, orthoTop, _orthographicNearClip, _orthographicFarClip);
}

} // namespace Codi
