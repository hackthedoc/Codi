#include "cdpch.h"
#include "Renderer2D.h"

#include "Codi/Renderer/RenderCommand.h"
#include "Codi/Renderer/VertexArray.h"
#include "Codi/Renderer/Shader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Codi {

struct QuadVertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    float TexIndex;
    float TilingFactor;
};

struct Renderer2DData {
    static const uint32_t MAX_QUADS = 50000;
    static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
    static const uint32_t MAX_INDICES = MAX_QUADS * 6;
    static const uint32_t MAX_TEXTURE_SLOTS = 32; // TODO: RenderCaps

    Ref<VertexArray> QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;

    Ref<Shader> TextureShader;
    Ref<Texture2D> WhiteTexture;

    uint32_t QuadIndexCount = 0;
    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;

    std::array<Ref<Texture2D>, MAX_TEXTURE_SLOTS> TextureSlots;
    uint32_t TextureSlotIndex = 1; // 0 = white texture

    glm::vec4 QuadVertexPositions[4];

    Renderer2D::Statistics Stats;
};
static Renderer2DData _Data;

void Renderer2D::Init() {
    CODI_PROFILE_FUNCTION();

    _Data.QuadVertexArray = VertexArray::Create();

    _Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MAX_VERTICES * sizeof(QuadVertex));
    _Data.QuadVertexBuffer->setLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color"    },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float,  "a_TexIndex" },
        { ShaderDataType::Float,  "a_TilingFactor" }
    });
    _Data.QuadVertexArray->addVertexBuffer(_Data.QuadVertexBuffer);

    _Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MAX_VERTICES];

    uint32_t* quadIndices = new uint32_t[Renderer2DData::MAX_INDICES];
    for (uint32_t i = 0, offset = 0; i < Renderer2DData::MAX_INDICES; i += 6, offset += 4) {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;
    }

    Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer2DData::MAX_INDICES);
    _Data.QuadVertexArray->setIndexBuffer(quadIB);

    delete[] quadIndices;

    uint32_t whiteTextureData = 0xffffffff;
    _Data.WhiteTexture = Texture2D::Create(1, 1, &whiteTextureData);

    int samplers[Renderer2DData::MAX_TEXTURE_SLOTS];
    for (uint32_t i = 0; i < Renderer2DData::MAX_TEXTURE_SLOTS; i++)
        samplers[i] = i;

    std::string vertexSrc = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;
        layout(location = 2) in vec2 a_TexCoord;
        layout(location = 3) in float a_TexIndex;
        layout(location = 4) in float a_TilingFactor;

        uniform mat4 u_ViewProjection;

        out vec4 v_Color;
        out vec2 v_TexCoord;
        out float v_TexIndex;
        out float v_TilingFactor;

        void main()
        {
            v_Color = a_Color;
            v_TexCoord = a_TexCoord;
            v_TexIndex = a_TexIndex;
            v_TilingFactor = a_TilingFactor;
            gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
        }
    )";
    std::string fragmentSrc = R"(
        #version 330 core

        layout(location = 0) out vec4 color;

        in vec4 v_Color;
        in vec2 v_TexCoord;
        in float v_TexIndex;
        in float v_TilingFactor;

        uniform sampler2D u_Texture[32];

        void main()
        {
            vec4 texColor = v_Color;
            switch (int(v_TexIndex))
            {
                case 0: texColor *= texture(u_Texture[0], v_TexCoord * v_TilingFactor); break;
                case 1: texColor *= texture(u_Texture[1], v_TexCoord * v_TilingFactor); break;
                case 2: texColor *= texture(u_Texture[2], v_TexCoord * v_TilingFactor); break;
                case 3: texColor *= texture(u_Texture[3], v_TexCoord * v_TilingFactor); break;
                case 4: texColor *= texture(u_Texture[4], v_TexCoord * v_TilingFactor); break;
                case 5: texColor *= texture(u_Texture[5], v_TexCoord * v_TilingFactor); break;
                case 6: texColor *= texture(u_Texture[6], v_TexCoord * v_TilingFactor); break;
                case 7: texColor *= texture(u_Texture[7], v_TexCoord * v_TilingFactor); break;
                case 8: texColor *= texture(u_Texture[8], v_TexCoord * v_TilingFactor); break;
                case 9: texColor *= texture(u_Texture[9], v_TexCoord * v_TilingFactor); break;
                case 10: texColor *= texture(u_Texture[10], v_TexCoord * v_TilingFactor); break;
                case 11: texColor *= texture(u_Texture[11], v_TexCoord * v_TilingFactor); break;
                case 12: texColor *= texture(u_Texture[12], v_TexCoord * v_TilingFactor); break;
                case 13: texColor *= texture(u_Texture[13], v_TexCoord * v_TilingFactor); break;
                case 14: texColor *= texture(u_Texture[14], v_TexCoord * v_TilingFactor); break;
                case 15: texColor *= texture(u_Texture[15], v_TexCoord * v_TilingFactor); break;
                case 16: texColor *= texture(u_Texture[16], v_TexCoord * v_TilingFactor); break;
                case 17: texColor *= texture(u_Texture[17], v_TexCoord * v_TilingFactor); break;
                case 18: texColor *= texture(u_Texture[18], v_TexCoord * v_TilingFactor); break;
                case 19: texColor *= texture(u_Texture[19], v_TexCoord * v_TilingFactor); break;
                case 20: texColor *= texture(u_Texture[20], v_TexCoord * v_TilingFactor); break;
                case 21: texColor *= texture(u_Texture[21], v_TexCoord * v_TilingFactor); break;
                case 22: texColor *= texture(u_Texture[22], v_TexCoord * v_TilingFactor); break;
                case 23: texColor *= texture(u_Texture[23], v_TexCoord * v_TilingFactor); break;
                case 24: texColor *= texture(u_Texture[24], v_TexCoord * v_TilingFactor); break;
                case 25: texColor *= texture(u_Texture[25], v_TexCoord * v_TilingFactor); break;
                case 26: texColor *= texture(u_Texture[26], v_TexCoord * v_TilingFactor); break;
                case 27: texColor *= texture(u_Texture[27], v_TexCoord * v_TilingFactor); break;
                case 28: texColor *= texture(u_Texture[28], v_TexCoord * v_TilingFactor); break;
                case 29: texColor *= texture(u_Texture[29], v_TexCoord * v_TilingFactor); break;
                case 30: texColor *= texture(u_Texture[30], v_TexCoord * v_TilingFactor); break;
                case 31: texColor *= texture(u_Texture[31], v_TexCoord * v_TilingFactor); break;
                default: texColor = v_Color; break;
            }
            color = texColor;
        }
    )";
    _Data.TextureShader = Shader::Create(vertexSrc, fragmentSrc);
    _Data.TextureShader->bind();
    _Data.TextureShader->setIntArray("u_Texture", samplers, Renderer2DData::MAX_TEXTURE_SLOTS);
    
    _Data.TextureSlots[0] = _Data.WhiteTexture;

    _Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    _Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
    _Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
    _Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
}

void Renderer2D::Shutdown() {
    CODI_PROFILE_FUNCTION();
}

void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform) {    
    CODI_PROFILE_FUNCTION();

    glm::mat4 viewProj = camera.getProjection() * glm::inverse(transform);
    
    _Data.TextureShader->bind();
    _Data.TextureShader->setMat4("u_ViewProjection", viewProj);

    _Data.QuadIndexCount = 0;
    _Data.QuadVertexBufferPtr = _Data.QuadVertexBufferBase;

    _Data.TextureSlotIndex = 1;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera) {    
    CODI_PROFILE_FUNCTION();
    
    _Data.TextureShader->bind();
    _Data.TextureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

    _Data.QuadIndexCount = 0;
    _Data.QuadVertexBufferPtr = _Data.QuadVertexBufferBase;

    _Data.TextureSlotIndex = 1;
}

void Renderer2D::EndScene() {
    CODI_PROFILE_FUNCTION();

    uint32_t dataSize = (uint8_t*)_Data.QuadVertexBufferPtr - (uint8_t*)_Data.QuadVertexBufferBase;
    _Data.QuadVertexBuffer->setData(_Data.QuadVertexBufferBase, dataSize);

    Flush();
}

void Renderer2D::Flush() {
    CODI_PROFILE_FUNCTION();

    for (uint32_t i = 0; i < _Data.TextureSlotIndex; i++)
        _Data.TextureSlots[i]->bind(i);
    
    RenderCommand::DrawIndexed(_Data.QuadVertexArray, _Data.QuadIndexCount);
    _Data.Stats.DrawCalls++;

    _Data.QuadIndexCount = 0;
}

void Renderer2D::FlushAndReset() {
    EndScene();

    _Data.QuadIndexCount = 0;
    _Data.QuadVertexBufferPtr = _Data.QuadVertexBufferBase;

    _Data.TextureSlotIndex = 1;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    DrawQuad({ position.x, position.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    DrawQuad(transform, color);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor) {
    DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    DrawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float tilingFactor, const glm::vec4& tintColor) {
    DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float tilingFactor, const glm::vec4& tintColor) {
    CODI_PROFILE_FUNCTION();

    if (_Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

    const Ref<Texture2D>& texture = subtexture->getTexture();
    const glm::vec2* texCoords = subtexture->getTexCoords();

    float textureIndex = 0.0f;

    for (uint32_t i = 1; i < _Data.TextureSlotIndex; i++) {
        if (*_Data.TextureSlots[i].get() == *texture.get()) {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f) {
        if (_Data.TextureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS)
            FlushAndReset();

        textureIndex = (float)_Data.TextureSlotIndex;
        _Data.TextureSlots[_Data.TextureSlotIndex] = texture;
        _Data.TextureSlotIndex++;
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[0];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = texCoords[0];
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[1];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = texCoords[1];
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[2];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = texCoords[2];
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[3];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = texCoords[3];
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadIndexCount += 6;

    _Data.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color) {
    CODI_PROFILE_FUNCTION();

    constexpr size_t quadVertexCount = 4;
    const float textureIndex = 0.0f; // White Texture
    constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
    const float tilingFactor = 1.0f;

    if (_Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

    for (size_t i = 0; i < quadVertexCount; i++) {
        _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[i];
        _Data.QuadVertexBufferPtr->Color = color;
        _Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
        _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        _Data.QuadVertexBufferPtr++;
    }

    _Data.QuadIndexCount += 6;

    _Data.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor) {
    CODI_PROFILE_FUNCTION();

    if (_Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

    float textureIndex = 0.0f;

    for (uint32_t i = 1; i < _Data.TextureSlotIndex; i++) {
        if (*_Data.TextureSlots[i].get() == *texture.get()) {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f) {
        if (_Data.TextureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS)
            FlushAndReset();

        textureIndex = (float)_Data.TextureSlotIndex;
        _Data.TextureSlots[_Data.TextureSlotIndex] = texture;
        _Data.TextureSlotIndex++;
    }
    
    constexpr size_t quadVertexCount = 4;
    constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
    
    for (size_t i = 0; i < quadVertexCount; i++) {
        _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[i];
        _Data.QuadVertexBufferPtr->Color = tintColor;
        _Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
        _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        _Data.QuadVertexBufferPtr++;
    }

    _Data.QuadIndexCount += 6;

    _Data.Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const glm::vec4& color) {
    DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const glm::vec4& color) {
    CODI_PROFILE_FUNCTION();
    
    if (_Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();
    
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                        * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
                        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    const float textureIndex = 0.0f; // White Texture
    const float tilingFactor = 1.0f;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[0];
    _Data.QuadVertexBufferPtr->Color = color;
    _Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[1];
    _Data.QuadVertexBufferPtr->Color = color;
    _Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[2];
    _Data.QuadVertexBufferPtr->Color = color;
    _Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[3];
    _Data.QuadVertexBufferPtr->Color = color;
    _Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadIndexCount += 6;

    _Data.Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor) {
    DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor, const glm::vec4& tintColor) {
    CODI_PROFILE_FUNCTION();

    if (_Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();
    
    float textureIndex = 0.0f;

    for (uint32_t i = 1; i < _Data.TextureSlotIndex; i++) {
        if (*_Data.TextureSlots[i].get() == *texture.get()) {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f) {
        if (_Data.TextureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS)
            FlushAndReset();

        textureIndex = (float)_Data.TextureSlotIndex;
        _Data.TextureSlots[_Data.TextureSlotIndex] = texture;
        _Data.TextureSlotIndex++;
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                        * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
                        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[0];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[1];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[2];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[3];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadIndexCount += 6;

    _Data.Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation, const Ref<SubTexture2D>& subtexture, const float tilingFactor, const glm::vec4& tintColor) {
    DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture, tilingFactor, tintColor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation, const Ref<SubTexture2D>& subtexture, const float tilingFactor, const glm::vec4& tintColor) {
    CODI_PROFILE_FUNCTION();

    if (_Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

    const Ref<Texture2D>& texture = subtexture->getTexture();
    const glm::vec2* texCoords = subtexture->getTexCoords();
    
    float textureIndex = 0.0f;

    for (uint32_t i = 1; i < _Data.TextureSlotIndex; i++) {
        if (*_Data.TextureSlots[i].get() == *texture.get()) {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f) {
        if (_Data.TextureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS)
            FlushAndReset();

        textureIndex = (float)_Data.TextureSlotIndex;
        _Data.TextureSlots[_Data.TextureSlotIndex] = texture;
        _Data.TextureSlotIndex++;
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                        * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
                        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[0];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = texCoords[0];
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[1];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = texCoords[1];
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[2];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = texCoords[2];
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadVertexBufferPtr->Position = transform * _Data.QuadVertexPositions[3];
    _Data.QuadVertexBufferPtr->Color = tintColor;
    _Data.QuadVertexBufferPtr->TexCoord = texCoords[3];
    _Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    _Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
    _Data.QuadVertexBufferPtr++;

    _Data.QuadIndexCount += 6;

    _Data.Stats.QuadCount++;
}

void Renderer2D::ResetStats() {
    memset(&_Data.Stats, 0, sizeof(Statistics));
}

Renderer2D::Statistics Renderer2D::GetStats() {
    return _Data.Stats;
}

} // namespace Codi
