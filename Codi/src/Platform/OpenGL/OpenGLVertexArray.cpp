#include "cdpch.h"
#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Codi {

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::Float  : return GL_FLOAT;
    case ShaderDataType::Float2 : return GL_FLOAT;
    case ShaderDataType::Float3 : return GL_FLOAT;
    case ShaderDataType::Float4 : return GL_FLOAT;
    case ShaderDataType::Mat3   : return GL_FLOAT;
    case ShaderDataType::Mat4   : return GL_FLOAT;
    case ShaderDataType::Int    : return GL_INT;
    case ShaderDataType::Int2   : return GL_INT;
    case ShaderDataType::Int3   : return GL_INT;
    case ShaderDataType::Int4   : return GL_INT;
    case ShaderDataType::Bool   : return GL_BOOL;
    }

    CODI_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

OpenGLVertexArray::OpenGLVertexArray() {
    CODI_PROFILE_FUNCTION();

    glCreateVertexArrays(1, &_rendererID);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    CODI_PROFILE_FUNCTION();
    
    glDeleteVertexArrays(1, &_rendererID);
}

void OpenGLVertexArray::bind() const {
    CODI_PROFILE_FUNCTION();
    
    glBindVertexArray(_rendererID);
}

void OpenGLVertexArray::unbind() const {
    CODI_PROFILE_FUNCTION();
    
    glBindVertexArray(0);
}

void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
    CODI_PROFILE_FUNCTION();
    
    bind();
    vertexBuffer->bind();

    CODI_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout!");

    int index = 0;
    const BufferLayout& layout = vertexBuffer->getLayout();
    for (const BufferElement& element : layout.getElements()) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              element.getComponentCount(),
                              ShaderDataTypeToOpenGLBaseType(element.type),
                              element.normalized ? GL_TRUE : GL_FALSE,
                              layout.getStride(),
                              (const void*)(uintptr_t)element.offset);
        index++;
    }

    _vertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
    CODI_PROFILE_FUNCTION();
    
    bind();
    indexBuffer->bind();

    _indexBuffer = indexBuffer;
}
    
} // namespace Codi
