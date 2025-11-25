#include "codipch.h"
#include "TextureManager.h"

namespace Codi {

    std::unordered_map<UUID, Shared<Texture2D>> TextureManager::_TexturesByUUID;
    std::unordered_map<std::string, Shared<Texture2D>> TextureManager::_TexturesByPath;

    void TextureManager::Init() {
        _TexturesByUUID.clear();
        _TexturesByPath.clear();

        CODI_CORE_INFO("Texture Manager initialized!");
    }

    void TextureManager::Shutdown() {
        CODI_CORE_INFO("Cleaning Texture Manager...");

        for (auto& [key, texture] : _TexturesByUUID)
            texture->Destroy();

        _TexturesByUUID.clear();
        _TexturesByPath.clear();
    }

    Shared<Texture2D> TextureManager::Load(const std::string& path) {
        if (Exists(path)) return _TexturesByPath[path];

        Shared<Texture2D> texture = Texture2D::Create(path);
        _TexturesByUUID[texture->GetUUID()] = texture;
        _TexturesByPath[path] = texture;
        return texture;
    }

    Shared<Texture2D> TextureManager::Create(const uint32 width, uint32 height, void* data) {
        Shared<Texture2D> texture = Texture2D::Create(width, height, data);
        _TexturesByUUID[texture->GetUUID()] = texture;
        return texture;
    }

    bool TextureManager::Exists(const UUID uuid) {
        return _TexturesByUUID.count(uuid) > 0;
    }

    Shared<Texture2D> TextureManager::Get(const UUID uuid) {
        if (Exists(uuid)) return _TexturesByUUID[uuid];
        else nullptr;
    }

    bool TextureManager::Exists(const std::string& path) {
        return _TexturesByPath.count(path) > 0;
    }

    Shared<Texture2D> TextureManager::Get(const std::string& path) {
        if (Exists(path)) return _TexturesByPath[path];
        else Load(path);
    }

} // namespace Codi
