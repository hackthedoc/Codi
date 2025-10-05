#include "cdpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Codi {

SceneCamera::SceneCamera() {
    _recalculateProjection();
}

void SceneCamera::setOrthographic(float size, float nearClip, float farClip) {
    _orthographicSize = size;
    _orthographicNear = nearClip;
    _orthographicFar  = farClip;

    _recalculateProjection();
}

void SceneCamera::setViewportSize(uint32_t width, uint32_t height) {
    _aspectRatio = (float)width / (float)height;
    _recalculateProjection();
}

void SceneCamera::_recalculateProjection() {
    const float orthoLeft   = -_orthographicSize * 0.5f * _aspectRatio;
    const float orthoRight  =  _orthographicSize * 0.5f * _aspectRatio;
    const float orthoBottom = -_orthographicSize * 0.5f;
    const float orthoTop    =  _orthographicSize * 0.5f;

    _projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, _orthographicNear, _orthographicFar);
}

} // namespace Codi
