#pragma once

#include "Codi/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Codi {

class SubTexture2D {
public:
    static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1.0f, 1.0f });
    static Ref<SubTexture2D> CreateFromMinMax(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

    SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2* texCoords);
    
    const Ref<Texture2D>& getTexture() const { return _texture; }
    const glm::vec2* getTexCoords() const { return _texCoords; }
private:
    Ref<Texture2D> _texture;
    glm::vec2 _texCoords[4];
};

} // namespace Codi

