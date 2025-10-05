#include "cdpch.h"
#include "OpenGLBuffer.h"

#include "glad/glad.h"

namespace Codi {
    
/* -------------------- VERTEX BUFFER -------------------- */

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
    CODI_PROFILE_FUNCTION();
    
    glCreateBuffers(1, &_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}
    
OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
    CODI_PROFILE_FUNCTION();
    
    glCreateBuffers(1, &_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    CODI_PROFILE_FUNCTION();
    
    glDeleteBuffers(1, &_rendererID);
}

void OpenGLVertexBuffer::bind() const {
    CODI_PROFILE_FUNCTION();
    
    glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
}

void OpenGLVertexBuffer::unbind() const {
    CODI_PROFILE_FUNCTION();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::setData(const void* data, uint32_t size) {
    CODI_PROFILE_FUNCTION();
    
    glBindBuffer(GL_ARRAY_BUFFER, _rendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

/* -------------------- INDEX BUFFER -------------------- */
    
OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
    : _count(count) {
    CODI_PROFILE_FUNCTION();
    
    glCreateBuffers(1, &_rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
    CODI_PROFILE_FUNCTION();
    
    glDeleteBuffers(1, &_rendererID);
}

void OpenGLIndexBuffer::bind() const {
    CODI_PROFILE_FUNCTION();
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererID);
}

void OpenGLIndexBuffer::unbind() const {
    CODI_PROFILE_FUNCTION();
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace Codi

