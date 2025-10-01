#include "cdpch.h"
#include "SubTexture.h"

namespace Codi {

Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize) {
    glm::vec2 min = { (coords.x * cellSize.x) / texture->getWidth(), (coords.y * cellSize.y) / texture->getHeight() };
    glm::vec2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texture->getWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture->getHeight() };
    return CreateFromMinMax(texture, min, max);
}

Ref<SubTexture2D> SubTexture2D::CreateFromMinMax(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max) {
    glm::vec2 texCoords[4] = {
        { min.x, min.y },
        { max.x, min.y },
        { max.x, max.y },
        { min.x, max.y }
    };
    return std::make_shared<SubTexture2D>(texture, texCoords);
}

SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2* texCoords)
    : _texture(texture) {
    memcpy(_texCoords, texCoords, 4 * sizeof(glm::vec2));
}



} // namespace Codi
