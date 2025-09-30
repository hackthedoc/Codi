#pragma once

#include "glm/glm.hpp"

namespace Codi {

class Shader {
public:
    virtual ~Shader() = default;

    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual void setInt(const std::string& name, const int value) = 0;
    virtual void setFloat(const std::string& name, const float value) = 0;
    virtual void setFloat4(const std::string& name,const glm::vec4& value) = 0;
    virtual void setMat4(const std::string& name, const glm::mat4& matrix) = 0;

    static Ref<Shader> Create(const std::string& filepath);
    static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
};

} // namespace Codi
