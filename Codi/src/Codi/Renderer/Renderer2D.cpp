#include "cdpch.h"
#include "Renderer2D.h"

#include "Codi/Renderer/RenderCommand.h"
#include "Codi/Renderer/VertexArray.h"
#include "Codi/Renderer/Shader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Codi {

struct Renderer2DData {
    Ref<VertexArray> SquareVertexArray;
    Ref<Shader> TextureShader;
    Ref<Texture2D> WhiteTexture;
};

static Renderer2DData* _Data;

void Renderer2D::Init() {
    _Data = new Renderer2DData();

    _Data->SquareVertexArray = VertexArray::Create();
    float squareVertices[5 * 4] = {
        -0.75f, -0.75f, 0.0f, 0.0f, 0.0f,
         0.75f, -0.75f, 0.0f, 1.0, 0.0f,
         0.75f,  0.75f, 0.0f, 1.0f, 1.0f,
        -0.75f,  0.75f, 0.0f, 0.0f, 1.0f
    };

    Ref<VertexBuffer> squareVB;
    squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
    squareVB->setLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" }
    });
    _Data->SquareVertexArray->addVertexBuffer(squareVB);

    uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
    Ref<IndexBuffer> squareIB;
    squareIB.reset(IndexBuffer::Create(squareIndices, 6));
    _Data->SquareVertexArray->setIndexBuffer(squareIB);

    _Data->TextureShader = Shader::Create("assets/shaders/TextureShader.glsl");
    _Data->TextureShader->bind();
    _Data->TextureShader->setInt("u_Texture", 0);

    uint32_t whiteTextureData = 0xffffffff;
    _Data->WhiteTexture = Texture2D::Create(1, 1, &whiteTextureData);
}

void Renderer2D::Shutdown() {
    delete _Data;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera) {    
    _Data->TextureShader->bind();
    _Data->TextureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
}

void Renderer2D::EndScene() {

}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    DrawQuad({ position.x, position.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
    _Data->TextureShader->setFloat4("u_Color", color);
    _Data->WhiteTexture->bind();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    _Data->TextureShader->setMat4("u_Transform", transform);
    
    _Data->SquareVertexArray->bind();
    RenderCommand::DrawIndexed(_Data->SquareVertexArray);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor) {
    DrawQuad({ position.x, position.y, 0.0f }, size, texture, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& tintColor) {
    texture->bind();

    _Data->TextureShader->setFloat4("u_Color", tintColor);

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    _Data->TextureShader->setMat4("u_Transform", transform);
    
    _Data->SquareVertexArray->bind();
    RenderCommand::DrawIndexed(_Data->SquareVertexArray);

    texture->unbind();
}

} // namespace Codi
