#include "cdpch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Codi { 

namespace Math {

bool DecomposeTransform(const glm::mat4& transform, glm::vec3& otranslation, glm::vec3& orotation, glm::vec3& oscale) {
    using namespace glm;

    mat4 localMatrix = transform;

    // Early out: invalid matrix
    if (epsilonEqual(localMatrix[3][3], static_cast<float>(0.0f), epsilon<float>()))
        return false;

    // Use glm::decompose which handles shear, negative scales, etc.
    vec3 skew;
    vec4 perspective;
    quat orientation;

    // glm::decompose expects column-major (glm), so this is fine
    bool ok = decompose(localMatrix, oscale, orientation, otranslation, skew, perspective);
    if (!ok)
        return false;

    // Convert orientation (quat) to Euler angles in radians
    orotation = eulerAngles(orientation); // returns radians

    // Fix possible negative scale signs:
    // glm::decompose may return negative scale for mirrored transforms.
    // We'll enforce positive scales and preserve transform shape by applying abs and clamping very small values.
    const float MIN_SCALE = 0.0001f;
    oscale = glm::abs(oscale);
    oscale = glm::max(oscale, glm::vec3(MIN_SCALE));

    return true;
}

} // namespace Math 

} // namespace Codi 
