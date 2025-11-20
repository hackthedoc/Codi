#pragma once

#include <glm/glm.hpp>

namespace Codi {

    class Camera {
    public:
        Camera() = default;
        Camera(const glm::mat4& projection) : _Projection(projection) {}
        virtual ~Camera() = default;

        const glm::mat4& GetProjection() const { return _Projection; }

    protected:
        glm::mat4 _Projection{ glm::mat4(1.0f) };
    };

} // namespace Codi
