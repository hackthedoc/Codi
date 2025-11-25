#include "codipch.h"
#include "Renderer2D.h"

#include "Codi/Renderer/Renderer.h"
#include "Codi/Renderer/Shader.h"
#include "Codi/Renderer/VertexArray.h"
#include "Codi/Renderer/TextureManager.h"

namespace Codi {

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec2 TexCoord;
    };

    struct QuadInstanceData {
        glm::mat4 Model;
        glm::vec4 Color;
        float32 TexIndex;
        glm::vec2 TilingFactor;
        int32 EntityID;
        float32 Padding[1]; // padding to 16-byte alignment
    };

    struct Renderer2DData {
        static const uint32 MAX_QUADS = 20000;
        static const uint32 MAX_VERTICES = MAX_QUADS * 4;
        static const uint32 MAX_INDICES = MAX_QUADS * 6;
        static const uint32 MAX_TEXTURE_SLOTS = 32; // TODO: RenderCaps

        Shared<VertexArray> QuadVertexArray = nullptr;
        Shared<VertexBuffer> QuadVertexBuffer = nullptr;
        Shared<ShaderStorageBuffer> QuadInstanceSSBO;
        Shared<Shader> QuadShader = nullptr;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;
        QuadInstanceData* QuadInstanceBufferBase = nullptr;
        QuadInstanceData* QuadInstanceBufferPtr = nullptr;

        std::array<Shared<Texture2D>, MAX_TEXTURE_SLOTS> TextureSlots;
        uint32 TextureSlotIndex = 1; // 0 = white texture
        Shared<Texture2D> WhiteTexture;

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
        // White Texture
        uint32 whiteData = 0xfffffffff;
        Data.WhiteTexture = TextureManager::Create(1, 1, &whiteData);
        Data.TextureSlots[0] = Data.WhiteTexture;
        
        // Create camera
        Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

        // Quads
        Data.QuadVertexArray = VertexArray::Create();

        Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MAX_VERTICES * sizeof(QuadVertex));
        Data.QuadVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position"      },
            { ShaderDataType::Float2, "a_TexCoord"      },
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
        for (auto& tex : Data.TextureSlots)
            tex.reset();
        Data.WhiteTexture.reset();

        Data.CameraUniformBuffer.reset();

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

        Data.TextureSlotIndex = 1;
    }

    void Renderer2D::Flush() {

        if (Data.QuadIndexCount) {
            uint32 dataSize = (uint32)((uint8*)Data.QuadVertexBufferPtr - (uint8*)Data.QuadVertexBufferBase);
            Data.QuadVertexBuffer->SetData(Data.QuadVertexBufferBase, dataSize);

            uint32 ssboSize = (uint32)((uint8*)Data.QuadInstanceBufferPtr - (uint8*)Data.QuadInstanceBufferBase);
            Data.QuadInstanceSSBO->SetData(Data.QuadInstanceBufferBase, ssboSize);

            for (uint32 i = 0; i < Data.TextureSlotIndex; i++)
                Data.TextureSlots[i]->Bind(i);

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

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, const int32 entityID) {
        if (Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        for (uint64 i = 0; i < 4; i++) {
            Data.QuadVertexBufferPtr->Position = transform * Data.QuadVertexPositions[i];
            Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            Data.QuadVertexBufferPtr++;
        }

        QuadInstanceData& instance = *Data.QuadInstanceBufferPtr++;
        instance.Model = transform;
        instance.Color = color;
        instance.TexIndex = 0.0f;
        instance.TilingFactor = glm::vec2(1.0f);
        instance.EntityID = entityID;

        Data.QuadIndexCount += 6;
        Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Shared<Texture2D>& texture, const float32 tilingFactor, const glm::vec4& tintColor, const int32 entityID) {
        if (Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

        float32 textureIndex = 0.0f;
        for (uint32 i = 1; i < Data.TextureSlotIndex; i++)
            if (*Data.TextureSlots[i] == *texture) { textureIndex = (float32)i; break; }

        if (textureIndex == 0.0f) {
            if (Data.TextureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS)
                FlushAndReset();
            textureIndex = (float32)Data.TextureSlotIndex;
            Data.TextureSlots[Data.TextureSlotIndex++] = texture;
        }

        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        for (uint64 i = 0; i < 4; i++) {
            Data.QuadVertexBufferPtr->Position = transform * Data.QuadVertexPositions[i];
            Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            Data.QuadVertexBufferPtr++;
        }

        QuadInstanceData& instance = *Data.QuadInstanceBufferPtr++;
        instance.Model = transform;
        instance.Color = tintColor;
        instance.TexIndex = textureIndex;
        instance.TilingFactor = glm::vec2(tilingFactor);
        instance.EntityID = entityID;

        Data.QuadIndexCount += 6;
        Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Shared<Material>& material, const int32 entityID) {
        if (Data.QuadIndexCount >= Renderer2DData::MAX_INDICES) FlushAndReset();

        Shared<Texture2D> albedo = Data.WhiteTexture;
        glm::vec4 color = glm::vec4(1.0f);
        glm::vec2 tilingFactor = glm::vec2(1.0f);

        if (material) {
            albedo = material->GetAlbedo();
            color = material->GetColor();
            tilingFactor = material->GetTilingFactor();
        }

        float32 textureIndex = 0.0f;
        if (albedo) {
            for (uint32 i = 1; i < Data.TextureSlotIndex; i++)
                if (*Data.TextureSlots[i] == *albedo) { textureIndex = (float32)i; break; }

            if (textureIndex == 0.0f) {
                if (Data.TextureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS)
                    FlushAndReset();
                textureIndex = (float32)Data.TextureSlotIndex;
                Data.TextureSlots[Data.TextureSlotIndex++] = albedo;
            }
        }

        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        for (uint64 i = 0; i < 4; i++) {
            Data.QuadVertexBufferPtr->Position = transform * Data.QuadVertexPositions[i];
            Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            Data.QuadVertexBufferPtr++;
        }

        QuadInstanceData& instance = *Data.QuadInstanceBufferPtr++;
        instance.Model = transform;
        instance.Color = color;
        instance.TexIndex = textureIndex;
        instance.TilingFactor = tilingFactor;
        instance.EntityID = entityID;

        Data.QuadIndexCount += 6;
        Data.Stats.QuadCount++;
    }

    Shared<Texture2D> Renderer2D::GetTextureSlot(uint32 index) {
        if (index < Data.TextureSlotIndex) return Data.TextureSlots[index];
        else return Data.WhiteTexture;
    }

    void Renderer2D::ResetStatistics() {
        Data.Stats.Reset();
    }

    Renderer2D::Statistics Renderer2D::GetStatistics() {
        return Data.Stats;
    }

} // namespace Codi
