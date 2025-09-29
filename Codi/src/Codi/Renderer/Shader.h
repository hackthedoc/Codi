#pragma once

#include "glm/glm.hpp"

namespace Codi {

class Shader {
public:
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();

    void bind() const;
    void unbind() const;

    void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

    inline uint getID() const { return _rendererID; }
private:
    uint _rendererID;
};

} // namespace Codi
