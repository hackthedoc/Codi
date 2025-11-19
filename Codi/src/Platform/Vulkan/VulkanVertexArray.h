#pragma once

#include "Codi/Renderer/VertexArray.h"

namespace Codi {

    class VulkanVertexArray : public VertexArray {
    public:
        VulkanVertexArray() = default;
        ~VulkanVertexArray() = default;

        void Bind() const override {}
        void Unbind() const override {}

        void AddVertexBuffer(const Shared<VertexBuffer>& vb) override { _VertexBuffers.push_back(vb); }

        void SetIndexBuffer(const Shared<IndexBuffer>& ib) override { _IndexBuffer = ib; }

        const std::vector<Shared<VertexBuffer>>& GetVertexBuffers() const override { return _VertexBuffers; }

        const Shared<IndexBuffer>& GetIndexBuffer() const override { return _IndexBuffer; }

    private:
        std::vector<Shared<VertexBuffer>> _VertexBuffers;
        Shared<IndexBuffer> _IndexBuffer;
    };

} // namespace Codi
