#pragma once

#include "Codi/Core/Core.h"
#include "Codi/Renderer/Texture.h"

#include <unordered_map>

namespace Codi {

    class TextureManager {
    public:
        static void Init();
        static void Shutdown();

        static Shared<Texture2D> Load(const std::string& path);
        static Shared<Texture2D> Create(const uint32 width, uint32 height, void* data = nullptr);

        static bool Exists(const UUID& uuid);
        static Shared<Texture2D> Get(const UUID& uuid);

        static bool Exists(const std::string& path);
        static Shared<Texture2D> Get(const std::string& path);

    private:
        TextureManager() = default;

        static std::unordered_map<UUID, Shared<Texture2D>> _TexturesByUUID;
        static std::unordered_map<std::string, Shared<Texture2D>> _TexturesByPath;
    };

} // namespace Codi
