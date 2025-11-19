#include "codipch.h"
#include "Renderer2D.h"

#include "Codi/Renderer/Renderer.h"
#include "Codi/Renderer/Shader.h"
#include "Codi/Renderer/VertexArray.h"

namespace Codi {

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
    };

    struct Renderer2DData {
        static const uint32 MAX_QUADS = 20000;
        static const uint32 MAX_VERTICES = MAX_QUADS * 4;
        static const uint32 MAX_INDICES = MAX_QUADS * 6;
        static const uint32 MAX_TEXTURE_SLOTS = 32; // TODO: RenderCaps

        Shared<VertexArray> QuadVertexArray;
        Shared<VertexBuffer> QuadVertexBuffer;
        Shared<Shader> QuadShader;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        glm::vec4 QuadVertexPositions[4];
    };
    static Renderer2DData Data;

    void Renderer2D::Init() {
        // Quads
        Data.QuadVertexArray = VertexArray::Create();

        Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MAX_VERTICES * sizeof(QuadVertex));
        Data.QuadVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position"      },
            { ShaderDataType::Float4, "a_Color"         },
        });
        Data.QuadVertexArray->AddVertexBuffer(Data.QuadVertexBuffer);
        Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MAX_VERTICES];

        uint32* quadIndices = new uint32[Renderer2DData::MAX_INDICES];
        for (uint32 i = 0, offset = 0; i < Renderer2DData::MAX_INDICES; i += 6, offset += 4) {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
        }

        Shared<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer2DData::MAX_INDICES);
        Data.QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        Data.QuadShader = Shader::Create("Resources/Shaders/Renderer2D_Quad.glsl");
        Data.QuadShader->CreatePipeline(Data.QuadVertexBuffer);

        // Set quad vertex positions
        Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
        Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }
    
    void Renderer2D::Shutdown() {
        delete[] Data.QuadVertexBufferBase;
        Data.QuadVertexBuffer = nullptr;
        Data.QuadVertexArray = nullptr;
        Data.QuadShader = nullptr;
    }

    void Renderer2D::StartBatch() {
        Data.QuadIndexCount = 0;
        Data.QuadVertexBufferPtr = Data.QuadVertexBufferBase;
    }

    void Renderer2D::Flush() {

        if (Data.QuadIndexCount) {
            uint32 dataSize = (uint32)((uint8*)Data.QuadVertexBufferPtr - (uint8*)Data.QuadVertexBufferBase);
            Data.QuadVertexBuffer->SetData(Data.QuadVertexBufferBase, dataSize);

            Data.QuadShader->Bind();
            Renderer::DrawIndexed(Data.QuadVertexArray, Data.QuadIndexCount);
        }
    }

    void Renderer2D::FlushAndReset() {
        Flush();
        StartBatch();
    }

    void Renderer2D::DrawQuad(const glm::vec4& color) {
        constexpr uint64 quadVertexCount = 4;
        const float textureIndex = 0.0f; // White Texture
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const float tilingFactor = 1.0f;

        if (Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

        for (uint64 i = 0; i < quadVertexCount; i++) {
            Data.QuadVertexBufferPtr->Position = Data.QuadVertexPositions[i];
            Data.QuadVertexBufferPtr->Color = color;
            Data.QuadVertexBufferPtr++;
        }

        Data.QuadIndexCount += 6;
    }

} // namespace Codi
