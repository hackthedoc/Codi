#include "cdpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace Codi {

static GLenum ShaderTypeFromString(const std::string& type) {
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    CODI_CORE_ASSERT(false, "Unknown shader type!");
    return GL_NONE;
}

OpenGLShader::OpenGLShader(const std::string& filepath) {
    std::string raw = readFile(filepath);
    std::unordered_map<GLenum, std::string> src = preprocess(raw);
    compile(src);
}

OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;
    compile(sources);
}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(_rendererID);
}

std::string OpenGLShader::readFile(const std::string& filepath) {
    std::ifstream infile(filepath, std::ios::binary);
    if (!infile) {
        CODI_CORE_ERROR("Could not open file '{0}'", filepath);
        return "";
    }

    std::string result;
    infile.seekg(0, std::ios::end);
    result.resize(infile.tellg());
    infile.seekg(0, std::ios::beg);
    infile.read(&result[0], result.size());
    infile.close();
    return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::preprocess(const std::string& source) {
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken("#type");
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos) {
        size_t eol = source.find_first_of("\r\n", pos);
        CODI_CORE_ASSERT(eol != std::string::npos, "Syntax error");
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        CODI_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specified!");

        size_t nextLinePos = source.find_first_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        size_t shaderLength = (pos == std::string::npos) ? std::string::npos : pos - nextLinePos;
        shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, shaderLength);
    }

    return shaderSources;
}

void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
    _rendererID = glCreateProgram();
    std::vector<GLenum> glShaderIDs;
    glShaderIDs.reserve(shaderSources.size());

    for (const std::pair<GLenum, std::string>& kv: shaderSources) {
        GLenum type = kv.first;
        const std::string& source = kv.second;

        uint shader = glCreateShader(type);
            
        const char* srcCstr = source.c_str();
        int length = (int)source.size();
        CINFO("\n{0}", srcCstr);
        glShaderSource(shader, 1, &srcCstr, &length);
        
        glCompileShader(shader);
        
        int isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        CWARN("Is Compiled: {0}", isCompiled);
        if (isCompiled == GL_FALSE) {
            int maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<char> infoLog(maxLength);
            if (maxLength > 0) {
                glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());
                CODI_CORE_ERROR("Shader compilation failed:\n{0}", infoLog.data());
            } else {
                CODI_CORE_ERROR("Shader compilation failed: No info log available.");
            }

            glDeleteShader(shader);
            CODI_CORE_ERROR("{0}", infoLog.data());
            CODI_CORE_ASSERT(false, "OpenGLShader compilation failure!");
            return;
        }
    
        glAttachShader(_rendererID, shader);
        glShaderIDs.push_back(shader);
    }

    glLinkProgram(_rendererID);
    
    int isLinked = 0;
    glGetProgramiv(_rendererID, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        int maxLength = 0;
        glGetProgramiv(_rendererID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> infoLog(maxLength);
        glGetProgramInfoLog(_rendererID, maxLength, &maxLength, &infoLog[0]);

        glDeleteProgram(_rendererID);
        
        for (GLenum id: glShaderIDs)
            glDeleteShader(id);

        CODI_CORE_ERROR("{0}", infoLog.data());
        CODI_CORE_ASSERT(false, "OpenGLShader link failure!");
        return;
    }

    for (GLenum id: glShaderIDs)
        glDetachShader(_rendererID, id);
}

void OpenGLShader::bind() const {
    glUseProgram(_rendererID);
}

void OpenGLShader::unbind() const {
    glUseProgram(0);
}

void OpenGLShader::uploadUniformInt(const std::string& name, const int value) {
    GLint location = glGetUniformLocation(_rendererID, name.c_str());
    glUniform1i(location, value);
}

void OpenGLShader::uploadUniformFloat(const std::string& name, const float value) {
    GLint location = glGetUniformLocation(_rendererID, name.c_str());
    glUniform1f(location, value);
}

void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value) {
    GLint location = glGetUniformLocation(_rendererID, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value) {
    GLint location = glGetUniformLocation(_rendererID, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value) {
    GLint location = glGetUniformLocation(_rendererID, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
    GLint location = glGetUniformLocation(_rendererID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
    GLint location = glGetUniformLocation(_rendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

} // namespace Codi