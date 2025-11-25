#include "codipch.h"
#include "BatchRenderer.h"

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
        float32 _padding; // padding to 16-byte alignment
    };

    struct QuadBatchStorage {
        static const uint32 MAX_QUADS = 20000;
        static const uint32 MAX_VERTICES = MAX_QUADS * 4;
        static const uint32 MAX_INDICES = MAX_QUADS * 6;
        static const uint32 MAX_TEXTURE_SLOTS = 32; // TODO: RenderCaps

        Shared<VertexArray> VertexArray;
        Shared<VertexBuffer> VertexBuffer;
        Shared<ShaderStorageBuffer> InstanceSSBO;
        Shared<Shader> Shader;

        QuadVertex* VertexBase = nullptr;
        QuadVertex* VertexPtr = nullptr;

        QuadInstanceData* InstanceBase = nullptr;
        QuadInstanceData* InstancePtr = nullptr;

        uint32 Count = 0; // index count ?

        std::array<Shared<Texture2D>, MAX_TEXTURE_SLOTS> TextureSlots;
        uint32 TextureSlotCount = 1; // slot 0 = white texture
        Shared<Texture2D> WhiteTexture;

        glm::vec4 Positions[4];
    };
    static QuadBatchStorage QuadData;

    void QuadBatchRenderer::Init() {
        CODI_CORE_WARN("QuadBatchRenderer::Init()");
        uint32 whiteData = 0xffffffff;
        QuadData.WhiteTexture = TextureManager::Create(1, 1, &whiteData);
        QuadData.TextureSlots[0] = QuadData.WhiteTexture;

        QuadData.VertexArray = VertexArray::Create();

        QuadData.VertexBuffer = VertexBuffer::Create(QuadBatchStorage::MAX_VERTICES * sizeof(QuadVertex));
        QuadData.VertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position"      },
            { ShaderDataType::Float2, "a_TexCoord"      },
        });
        QuadData.VertexArray->AddVertexBuffer(QuadData.VertexBuffer);

        QuadData.VertexBase = new QuadVertex[QuadBatchStorage::MAX_VERTICES];
        QuadData.InstanceBase = new QuadInstanceData[QuadBatchStorage::MAX_QUADS];

        QuadData.InstanceSSBO = ShaderStorageBuffer::Create(sizeof(QuadInstanceData) * QuadBatchStorage::MAX_QUADS, 1);

        uint32* quadIndices = new uint32[QuadBatchStorage::MAX_INDICES];
        for (uint32 i = 0, offset = 0; i < QuadBatchStorage::MAX_INDICES; i += 6, offset += 4) {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
        }

        Shared<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, QuadBatchStorage::MAX_INDICES);
        QuadData.VertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        QuadData.Positions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        QuadData.Positions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        QuadData.Positions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
        QuadData.Positions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        QuadData.Shader = Shader::Create("Resources/Shaders/Renderer2D_Quad.glsl");
        QuadData.Shader->CreatePipeline(QuadData.VertexBuffer);
    }

    void QuadBatchRenderer::Shutdown() {
        delete[] QuadData.VertexBase;
        delete[] QuadData.InstanceBase;
    }

    void QuadBatchRenderer::Flush() {
        if (QuadData.Count == 0) return;

        uint32 dataSize = (uint32)((uint8*)QuadData.VertexPtr - (uint8*)QuadData.VertexBase);
        QuadData.VertexBuffer->SetData(QuadData.VertexBase, dataSize);

        uint32 ssboSize = (uint32)((uint8*)QuadData.InstancePtr - (uint8*)QuadData.InstanceBase);
        QuadData.InstanceSSBO->SetData(QuadData.InstanceBase, ssboSize);

        for (uint32 i = 0; i < QuadData.TextureSlotCount; i++)
            QuadData.TextureSlots[i]->Bind(i);

        uint32 instanceCount = (uint32)(QuadData.InstancePtr - QuadData.InstanceBase);

        QuadData.Shader->Bind();
        Renderer::DrawIndexed(QuadData.VertexArray, QuadData.Count, instanceCount);
    }

    void QuadBatchRenderer::StartBatch() {
        CODI_CORE_WARN("QuadBatchRenderer::StartBatch()");

        QuadData.Count = 0;
        QuadData.VertexPtr = QuadData.VertexBase;
        QuadData.InstancePtr = QuadData.InstanceBase;

        QuadData.TextureSlotCount = 1;
    }

    void QuadBatchRenderer::FlushAndReset() {
        Flush();
        StartBatch();
    }

    void QuadBatchRenderer::Submit(const glm::mat4& transform, const glm::vec4& color, const int32 entityID) {
        if (QuadData.Count >= QuadBatchStorage::MAX_INDICES) FlushAndReset();

        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        for (uint64 i = 0; i < 4; i++) {
            QuadData.VertexPtr->Position = transform * QuadData.Positions[i];
            QuadData.VertexPtr->TexCoord = textureCoords[i];
            QuadData.VertexPtr++;
        }

        QuadInstanceData& instance = *QuadData.InstancePtr++;
        instance.Model = transform;
        instance.Color = color;
        instance.TexIndex = 0.0f;
        instance.TilingFactor = glm::vec2(1.0f);
        instance.EntityID = entityID;

        QuadData.Count += 6;
    }

    void QuadBatchRenderer::Submit(const glm::mat4& transform, const Shared<Texture2D>& texture, const float32 tilingFactor, const glm::vec4& tintColor, const int32 entityID) {
        if (QuadData.Count >= QuadBatchStorage::MAX_INDICES) FlushAndReset();

        float32 textureIndex = GetTextureIndex(texture);

        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        for (uint64 i = 0; i < 4; i++) {
            QuadData.VertexPtr->Position = transform * QuadData.Positions[i];
            QuadData.VertexPtr->TexCoord = textureCoords[i];
            QuadData.VertexPtr++;
        }

        QuadInstanceData& instance = *QuadData.InstancePtr++;
        instance.Model = transform;
        instance.Color = tintColor;
        instance.TexIndex = textureIndex;
        instance.TilingFactor = glm::vec2(tilingFactor);
        instance.EntityID = entityID;

        QuadData.Count += 6;
    }

    void QuadBatchRenderer::Submit(const glm::mat4& transform, const Shared<Material>& material, const int32 entityID) {
        if (QuadData.Count >= QuadBatchStorage::MAX_INDICES) FlushAndReset();

        Shared<Texture2D> albedo = QuadData.WhiteTexture;
        glm::vec4 color = glm::vec4(1.0f);
        glm::vec2 tilingFactor = glm::vec2(1.0f);

        if (material) {
            albedo = material->GetAlbedo();
            color = material->GetColor();
            tilingFactor = material->GetTilingFactor();
        }

        float32 textureIndex = GetTextureIndex(albedo);

        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

        for (uint64 i = 0; i < 4; i++) {
            QuadData.VertexPtr->Position = transform * QuadData.Positions[i];
            QuadData.VertexPtr->TexCoord = textureCoords[i];
            QuadData.VertexPtr++;
        }

        QuadInstanceData& instance = *QuadData.InstancePtr++;
        instance.Model = transform;
        instance.Color = color;
        instance.TexIndex = textureIndex;
        instance.TilingFactor = tilingFactor;
        instance.EntityID = entityID;

        QuadData.Count += 6;
    }

    Shared<Texture2D> QuadBatchRenderer::GetTextureSlot(uint32 index) {
        if (index < QuadData.TextureSlotCount) return QuadData.TextureSlots[index];
        else return QuadData.WhiteTexture;
    }

    float32 QuadBatchRenderer::GetTextureIndex(const Shared<Texture2D>& texture) {
        float32 textureIndex = 0.0f;
        for (uint32 i = 1; i < QuadData.TextureSlotCount; i++)
            if (*QuadData.TextureSlots[i] == *texture) { textureIndex = (float32)i; break; }

        if (textureIndex == 0.0f) {
            if (QuadData.TextureSlotCount >= QuadBatchStorage::MAX_TEXTURE_SLOTS)
                FlushAndReset();
            textureIndex = (float32)QuadData.TextureSlotCount;
            QuadData.TextureSlots[QuadData.TextureSlotCount++] = texture;
        }

        return textureIndex;
    }

} // namespace Codi
