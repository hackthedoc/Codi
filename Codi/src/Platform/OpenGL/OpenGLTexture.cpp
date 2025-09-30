#include "cdpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Codi {

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, void* data)
    : _width(width), _height(height) 
    {
    CODI_PROFILE_FUNCTION();
    
    _internalFormat = GL_RGBA8;
     _dataFormat = GL_RGBA;
    
    glCreateTextures(GL_TEXTURE_2D, 1, &_rendererID);
    glTextureStorage2D(_rendererID, 1, _internalFormat, _width, _height);

    glTextureParameteri(_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (data)
        setData(data, width * height * 4);
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : _path(path) {
    CODI_PROFILE_FUNCTION();
    
    int w, h, c;
    stbi_set_flip_vertically_on_load(1);\
    stbi_uc* data = nullptr;
    {
        CODI_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
        data = stbi_load(_path.c_str(), &w, &h, &c, 0);
    }
    CODI_CORE_ASSERT(data, "Failed to load image!");
    _width = w;
    _height = h;

    GLenum _internalFormat = 0, _dataFormat = 0;
    if (c == 4) {
        _internalFormat = GL_RGBA8;
        _dataFormat = GL_RGBA;
    }
    else if (c == 3) {
        _internalFormat = GL_RGB8;
        _dataFormat = GL_RGB;
    }
    CODI_CORE_ASSERT(_internalFormat & _dataFormat, "Format not supported!");

    glCreateTextures(GL_TEXTURE_2D, 1, &_rendererID);
    glTextureStorage2D(_rendererID, 1, _internalFormat, _width, _height);

    glTextureParameteri(_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(_rendererID, 0, 0, 0, _width, _height, _dataFormat, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() {
    CODI_PROFILE_FUNCTION();
    
    glDeleteTextures(1, &_rendererID);
}

void OpenGLTexture2D::bind(uint32_t slot) const {
    CODI_PROFILE_FUNCTION();
    
    glBindTextureUnit(slot, _rendererID);
}

void OpenGLTexture2D::unbind() const {
    CODI_PROFILE_FUNCTION();
    
    glBindTextureUnit(0, 0);
}

void OpenGLTexture2D::setData(void* data, uint32_t size) {
    CODI_PROFILE_FUNCTION();
    
    uint32_t bpp = _dataFormat == GL_RGBA ? 4 : 3;
    CODI_CORE_ASSERT(size == _width * _height * bpp, "Data must be entire texture!");
    glTextureSubImage2D(_rendererID, 0, 0, 0, _width, _height, _dataFormat, GL_UNSIGNED_BYTE, data);
}

} // namespace Codi