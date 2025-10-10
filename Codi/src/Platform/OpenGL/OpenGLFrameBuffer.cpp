#include "cdpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Codi {

constexpr uint32_t MAX_FRAME_BUFFER_SIZE = 8192;

namespace Utils {

    static GLenum TextureTarget(bool multisampled) {
        if (multisampled) return GL_TEXTURE_2D_MULTISAMPLE;
        else return GL_TEXTURE_2D;
    }

    static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count) {
        glCreateTextures(TextureTarget(multisampled), count, outID);
    }

    static void BindTexture(bool multisampled, uint32_t id) {
        glBindTexture(TextureTarget(multisampled), id);
    }

    static void AttachColorTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height, int index) {
        bool multisampled = samples > 1;

        if (multisampled)
            glTextureStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
    }

    static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
        bool multisampled = samples > 1;

        if (multisampled)
            glTextureStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        else {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
    }

    static bool IsDepthFormat(FrameBufferTextureFormat format) {
        switch (format) {
        case FrameBufferTextureFormat::DEPTH24STENCIL8:
            return true;
        }

        return false;
    }

} // namespace Utils

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
    : _specification(spec) {
    CODI_PROFILE_FUNCTION();

    for (const FrameBufferTextureSpecification format : _specification.attachements.attachements) {
        if (Utils::IsDepthFormat(format.textureFormat))
            _depthAttachementSpec = format;
        else
            _colorAttachmentSpecs.push_back(format);
    }
        
    invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
    CODI_PROFILE_FUNCTION();

    glDeleteFramebuffers(1, &_rendererID);
    glDeleteTextures(_colorAttachments.size(), _colorAttachments.data());
    glDeleteTextures(1, &_depthAttachment);
    
    _colorAttachments.clear();
    _depthAttachment = 0;
}

void OpenGLFrameBuffer::invalidate() {
    CODI_PROFILE_FUNCTION();

    if (_rendererID) {
        glDeleteFramebuffers(1, &_rendererID);
        glDeleteTextures(_colorAttachments.size(), _colorAttachments.data());
        glDeleteTextures(1, &_depthAttachment);
        
        _colorAttachments.clear();
        _depthAttachment = 0;
    }

    // Create the frame buffer
    glCreateFramebuffers(1, &_rendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, _rendererID);

    const bool multisampled = _specification.samples > 1;

    // Attachements
    if (_colorAttachmentSpecs.size()) {
        _colorAttachments.resize(_colorAttachmentSpecs.size());
        Utils::CreateTextures(multisampled, _colorAttachments.data(), _colorAttachments.size());

        for (uint32_t i = 0; i < _colorAttachments.size(); i++) {
            Utils::BindTexture(multisampled, _colorAttachments[i]);

            switch (_colorAttachmentSpecs[i].textureFormat) {
                case FrameBufferTextureFormat::RGBA8:
                    Utils::AttachColorTexture(_colorAttachments[i], _specification.samples, GL_RGBA8, _specification.width, _specification.height, i);
                    break;
            }
        }
    }

    // Depth buffer
    if (_depthAttachementSpec.textureFormat != FrameBufferTextureFormat::None) {
        Utils::CreateTextures(multisampled, &_depthAttachment, 1);
        Utils::BindTexture(multisampled, _depthAttachment);
        switch (_depthAttachementSpec.textureFormat) {
        case FrameBufferTextureFormat::DEPTH24STENCIL8:
            Utils::AttachDepthTexture(_depthAttachment, _specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, _specification.width, _specification.height); 
            break;
        }
    }

    if (_colorAttachments.size() > 0) {
        CODI_CORE_ASSERT(_colorAttachments.size() <= 4);
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(_colorAttachments.size(), buffers);
    }
    else
        glDrawBuffer(GL_NONE); // Only depth-pass

    CODI_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, _rendererID);
}

void OpenGLFrameBuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height) {
    if (width <= 0 || width > MAX_FRAME_BUFFER_SIZE ||
        height <= 0 || height > MAX_FRAME_BUFFER_SIZE
    ) {
        CODI_CORE_WARN("Attempted to resize the framebuffer to {0}, {1}", width, height);
        return;
    }

    _specification.width = width;
    _specification.height = height;
    invalidate();
}

} // namespace Codi
