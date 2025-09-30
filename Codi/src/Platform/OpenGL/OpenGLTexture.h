#pragma once

#include "Codi/Renderer/Texture.h"

#include <glad/glad.h>

namespace Codi {

class OpenGLTexture2D : public Texture2D {
public:
    OpenGLTexture2D(uint32_t width, uint32_t height, void* data = nullptr);
    OpenGLTexture2D(const std::string& path);
    virtual ~OpenGLTexture2D();

    virtual uint32_t getWidth() const override { return _width; }
    virtual uint32_t getHeight() const override { return _height; }

    virtual void setData(void* data, uint32_t size) override;

    virtual void bind(uint32_t slot = 0) const override;
    virtual void unbind() const override;

private:
    std::string _path;
    uint32_t _width;
    uint32_t _height;
    uint32_t _rendererID;

    GLenum _internalFormat;
    GLenum _dataFormat;
};

} // namespace Codi
