#pragma once

namespace Codi {

class Shader {
public:
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();

    void bind() const;
    void unbind() const;

private:
    uint _rendererID;
};

} // namespace Codi
