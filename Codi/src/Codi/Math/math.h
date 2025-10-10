#pragma once

#include <glm/glm.hpp>

namespace Codi {        
    
namespace Math {

bool DecomposeTransform(const glm::mat4& transform, glm::vec3& otranslation, glm::vec3& orotation, glm::vec3& oscale);

} // namespace Math 

} // namespace Codi 
