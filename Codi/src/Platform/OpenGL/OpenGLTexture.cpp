#include "cdpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Codi {

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : _path(path) {
    int w, h, c;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = stbi_load(_path.c_str(), &w, &h, &c, 0);
    CODI_CORE_ASSERT(data, "Failed to load image!");
    _width = w;
    _height = h;

    GLenum internalFormat = 0, dataFormat = 0;
    if (c == 4) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGBA;
    }
    else if (c == 3) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }
    CODI_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

    glCreateTextures(GL_TEXTURE_2D, 1, &_rendererID);
    glTextureStorage2D(_rendererID, 1, internalFormat, _width, _height);

    glTextureParameteri(_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

    glTextureSubImage2D(_rendererID, 0, 0, 0, _width, _height, dataFormat, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() {
    glDeleteTextures(1, &_rendererID);
}

void OpenGLTexture2D::bind(uint32_t slot) const {
    glBindTextureUnit(slot, _rendererID);
}

} // namespace Codi