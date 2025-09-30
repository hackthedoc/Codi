#pragma once

#include <unordered_map>
#include <glad/glad.h>

#include "Codi/Renderer/Shader.h"

namespace Codi {

class OpenGLShader : public Shader {
public:
    OpenGLShader(const std::string& filepath);
    OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~OpenGLShader();

    virtual void bind() const override;
    virtual void unbind() const override;

    virtual void setInt(const std::string& name, const int value) override;
    virtual void setFloat(const std::string& name, const float value) override;
    virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
    virtual void setMat4(const std::string& name, const glm::mat4& matrix) override;

    void uploadUniformInt(const std::string& name, const int value);

    void uploadUniformFloat(const std::string& name, const float value);
    void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
    void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
    void uploadUniformFloat4(const std::string& name, const glm::vec4& value);

    void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
    void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
    std::string readFile(const std::string& filepath);
    std::unordered_map<GLenum, std::string> preprocess(const std::string& source);
    void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

    uint32_t _rendererID;
};

} // namespace Codi