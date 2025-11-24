#include "codipch.h"
#include "Renderer2D.h"

#include "Codi/Renderer/Renderer.h"
#include "Codi/Renderer/Shader.h"
#include "Codi/Renderer/VertexArray.h"

namespace Codi {

    struct QuadVertex {
        glm::vec3 Position;
    };

    struct QuadInstanceData {
        glm::mat4 Model;
        glm::vec3 Color;
        float32 TilingFactor;
        int32 EntityID;
        float32 Padding[3]; // padding to 16-byte alignment
    };

    struct Renderer2DData {
        static const uint32 MAX_QUADS = 20000;
        static const uint32 MAX_VERTICES = MAX_QUADS * 4;
        static const uint32 MAX_INDICES = MAX_QUADS * 6;
        static const uint32 MAX_TEXTURE_SLOTS = 32; // TODO: RenderCaps

        Shared<VertexArray> QuadVertexArray = nullptr;
        Shared<VertexBuffer> QuadVertexBuffer = nullptr;
        Shared<Shader> QuadShader = nullptr;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;
        QuadInstanceData* QuadInstanceBufferBase = nullptr;
        QuadInstanceData* QuadInstanceBufferPtr = nullptr;

        Shared<ShaderStorageBuffer> QuadInstanceSSBO;

        glm::vec4 QuadVertexPositions[4];

        struct CameraData {
            glm::mat4 ViewProjection = glm::mat4(0.0f);
        };
        CameraData CameraBuffer{};
        Shared<UniformBuffer> CameraUniformBuffer;

        Renderer2D::Statistics Stats;
    };
    static Renderer2DData Data;

    void Renderer2D::Init() {
        // Create camera
        Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

        // Quads
        Data.QuadVertexArray = VertexArray::Create();

        Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MAX_VERTICES * sizeof(QuadVertex));
        Data.QuadVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position"      },
        });
        Data.QuadVertexArray->AddVertexBuffer(Data.QuadVertexBuffer);
        Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MAX_VERTICES];

        // Quad SSBO
        Data.QuadInstanceBufferBase = new QuadInstanceData[Renderer2DData::MAX_QUADS];
        Data.QuadInstanceSSBO = ShaderStorageBuffer::Create(sizeof(QuadInstanceData) * Renderer2DData::MAX_QUADS, 1);

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
        Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }
    
    void Renderer2D::Shutdown() {
        Data.CameraUniformBuffer = nullptr;

        delete[] Data.QuadVertexBufferBase;
        delete[] Data.QuadInstanceBufferBase;
        Data.QuadInstanceSSBO = nullptr;
        Data.QuadVertexBuffer = nullptr;
        Data.QuadVertexArray = nullptr;
        Data.QuadShader = nullptr;
    }

    void Renderer2D::BeginScene(const EditorCamera& camera) {
        StartBatch();

        Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
        Data.CameraUniformBuffer->SetData(&Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
    }

    void Renderer2D::EndScene() {
        Flush();
    }

    void Renderer2D::StartBatch() {
        Data.QuadIndexCount = 0;
        Data.QuadVertexBufferPtr = Data.QuadVertexBufferBase;
        Data.QuadInstanceBufferPtr = Data.QuadInstanceBufferBase;
    }

    void Renderer2D::Flush() {

        if (Data.QuadIndexCount) {
            uint32 dataSize = (uint32)((uint8*)Data.QuadVertexBufferPtr - (uint8*)Data.QuadVertexBufferBase);
            Data.QuadVertexBuffer->SetData(Data.QuadVertexBufferBase, dataSize);

            uint32 ssboSize = (uint32)((uint8*)Data.QuadInstanceBufferPtr - (uint8*)Data.QuadInstanceBufferBase);
            Data.QuadInstanceSSBO->SetData(Data.QuadInstanceBufferBase, ssboSize);

            uint32 instanceCount = (uint32)(Data.QuadInstanceBufferPtr - Data.QuadInstanceBufferBase);

            Data.QuadShader->Bind();
            Renderer::DrawIndexed(Data.QuadVertexArray, Data.QuadIndexCount, instanceCount);
            Data.Stats.DrawCalls++;
        }
    }

    void Renderer2D::FlushAndReset() {
        Flush();
        StartBatch();
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color) {
        const float textureIndex = 0.0f; // White Texture
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const float tilingFactor = 1.0f;

        if (Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

        for (uint64 i = 0; i < 4; i++) {
            Data.QuadVertexBufferPtr->Position = transform * Data.QuadVertexPositions[i];
            Data.QuadVertexBufferPtr++;
        }

        QuadInstanceData& instance = *Data.QuadInstanceBufferPtr++;
        instance.Model = transform;
        instance.Color = color;
        instance.TilingFactor = 1.0f;
        instance.EntityID = -1;

        Data.QuadIndexCount += 6;
        Data.Stats.QuadCount++;
    }

    void Renderer2D::ResetStatistics() {
        Data.Stats.Reset();
    }

    Renderer2D::Statistics Renderer2D::GetStatistics() {
        return Data.Stats;
    }

} // namespace Codi
