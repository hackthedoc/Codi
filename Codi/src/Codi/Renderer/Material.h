#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Core/UUID.h"
#include "Codi/Renderer/Texture.h"

#include <unordered_map>
#include <glm/glm.hpp>

namespace Codi {

    class Material {
    public:
        virtual ~Material() = default;

        virtual void Destroy() = 0;

        virtual const UUID& GetUUID() const = 0;
        virtual const std::string& GetName() const = 0;

        virtual void SetAlbedo(const Shared<Texture2D>& texture) = 0;
        virtual void SetColor(const glm::vec4& color) = 0;
        virtual void SetTilingFactor(const glm::vec2& tilingFactor) = 0;

        virtual Shared<Texture2D> GetAlbedo() const = 0;
        virtual const glm::vec4& GetColor() const = 0;
        virtual const glm::vec2& GetTilingFactor() const = 0;

        virtual bool operator==(const Material& other) const = 0;
    };

    class MaterialStandard : public Material {
    public:
        MaterialStandard(const std::string& name);

        virtual ~MaterialStandard() override = default;

        virtual void Destroy() override;

        virtual const UUID& GetUUID() const override { return _UUID; }
        virtual const std::string& GetName() const override { return _Name; }

        virtual void SetAlbedo(const Shared<Texture2D>& texture) override { _Albedo = texture; }
        virtual void SetColor(const glm::vec4& color) override { _Color = color; }
        virtual void SetTilingFactor(const glm::vec2& tilingFactor) override { _TilingFactor = tilingFactor; }

        virtual Shared<Texture2D> GetAlbedo() const override { return _Albedo; }
        virtual const glm::vec4& GetColor() const override { return _Color; }
        virtual const glm::vec2& GetTilingFactor() const override { return _TilingFactor; }

        virtual bool operator==(const Material& other) const override { return _UUID == other.GetUUID(); }

        static Shared<MaterialStandard> Create(const std::string& name);

    private:
        UUID _UUID;
        std::string _Name;

        Shared<Texture2D> _Albedo;
        glm::vec4 _Color = glm::vec4(1.0f);
        glm::vec2 _TilingFactor = glm::vec2(1.0f);
    };

} // namespace Codi
