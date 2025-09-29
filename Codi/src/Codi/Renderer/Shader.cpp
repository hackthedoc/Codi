#include "cdpch.h"
#include "Shader.h"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace Codi {

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    // Create an empty vertex shader handle
    uint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL
    const char* source = vertexSrc.c_str();
    glShaderSource(vertexShader, 1, &source, 0);

    // Compile the vertex shader
    glCompileShader(vertexShader);

    int isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        int maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<char> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(vertexShader);

        CODI_CORE_ERROR("{0}", infoLog.data());
        CODI_CORE_ASSERT(false, "Vertex shader compilation failure!");
        return;
    }

    // Create an empty fragment shader handle
    uint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    source = fragmentSrc.c_str();
    glShaderSource(fragmentShader, 1, &source, 0);

    // Compile the fragment shader
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        int maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<char> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(fragmentShader);
        // Either of them. Don't leak shaders.
        glDeleteShader(vertexShader);

        CODI_CORE_ERROR("{0}", infoLog.data());
        CODI_CORE_ASSERT(false, "Fragment shader compilation failure!");
        return;
    }

    // Vertex and fragment shaders are successfully compiled.
    // Now create a program and link them.
    _rendererID = glCreateProgram();

    glAttachShader(_rendererID, vertexShader);
    glAttachShader(_rendererID, fragmentShader);

    glLinkProgram(_rendererID);
    
    // Note the different functions here: glGetProgram* instead of glGetShader*.
    int isLinked = 0;
    glGetProgramiv(_rendererID, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        int maxLength = 0;
        glGetProgramiv(_rendererID, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<char> infoLog(maxLength);
        glGetProgramInfoLog(_rendererID, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(_rendererID);
        // Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        CODI_CORE_ERROR("{0}", infoLog.data());
        CODI_CORE_ASSERT(false, "Shader link failure!");
        return;
    }

    // Always detach shaders after a successful link.
    glDetachShader(_rendererID, vertexShader);
    glDetachShader(_rendererID, fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(_rendererID);
}

void Shader::bind() const {
    glUseProgram(_rendererID);
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
    GLint location = glGetUniformLocation(_rendererID, name.c_str());
    CODI_CORE_ASSERT(location != -1, "Uniform {0} not found!", name);
    glUseProgram(_rendererID);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

} // namespace Codi