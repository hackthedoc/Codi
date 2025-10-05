#pragma once

#include "Codi/Renderer/Camera.h"

namespace Codi {

class SceneCamera : public Camera {
public:
    SceneCamera();
    virtual ~SceneCamera() = default;

    void setOrthographic(float size, float nearClip, float farClip);

    void setViewportSize(uint32_t width, uint32_t height);

private:
    void _recalculateProjection();

    float _orthographicSize = 10.0f;
    float _orthographicNear = -1.0f;
    float _orthographicFar  =  1.0f;

    float _aspectRatio = 0.0f;
};

} // namespace Codi
