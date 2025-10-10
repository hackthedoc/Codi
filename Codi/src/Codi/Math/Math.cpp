#include "cdpch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Codi { 

namespace Math {

bool DecomposeTransform(const glm::mat4& transform, glm::vec3& otranslation, glm::vec3& orotation, glm::vec3& oscale) {
    using namespace glm;
    using T = float;

    mat4 localMatrix(transform);

    if (epsilonEqual(localMatrix[3][3], static_cast<T>(0), epsilon<T>())) return false;

    if (
        epsilonNotEqual(localMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(localMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(localMatrix[2][3], static_cast<T>(0), epsilon<T>())
    ) {
        localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
        localMatrix[3][3] = static_cast<T>(1);
    }

    otranslation = vec3(localMatrix[3]);
    localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

    vec3 row[3];

    for (length_t i = 0; i < 3; i++)
        for (length_t j = 0; j < 3; j++)
            row[i][j] = localMatrix[i][j];

    oscale.x = length(row[0]);
    row[0] = detail::scale(row[0], static_cast<T>(1));
    oscale.y = length(row[1]);
    row[0] = detail::scale(row[1], static_cast<T>(1));
    oscale.z = length(row[2]);
    row[0] = detail::scale(row[2], static_cast<T>(1));

    orotation.y = asin(-row[0][2]);
    if (cos(orotation.y) != 0) {
        orotation.x = atan2(row[1][2], row[2][2]);
        orotation.z = atan2(row[0][1], row[0][0]);
    } else {
        orotation.x = atan2(-row[2][0], row[1][1]);
        orotation.z = 0;
    }

    return true;
}

} // namespace Math 

} // namespace Codi 
