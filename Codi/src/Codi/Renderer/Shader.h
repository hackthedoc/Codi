#pragma once

#include "Codi/Core/Core.h"

#include "Codi/Renderer/Buffer.h"

#include <glm/glm.hpp>

namespace Codi {

    class Shader {
    public:
        enum class Type {
            None = 0,
            Vertex,
            Fragment
        };

    public:
        virtual ~Shader() = default;

        virtual void Bind() = 0;
        virtual void CreatePipeline(Shared<VertexBuffer> quadVertexBuffer) = 0;

        virtual void SetInt(const std::string& name, const int value) = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
        virtual void SetFloat(const std::string& name, const float value) = 0;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& matrix) = 0;

        virtual const std::string& GetName() const = 0;

        static Shared<Shader> Create(const std::filesystem::path& filepath);
        static Shared<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    };

    namespace Utils {

        static Shader::Type ShaderTypeFromString(const std::string& type) {
            if (type == "vertex") return Shader::Type::Vertex;
            if (type == "fragment" || type == "pixel") return Shader::Type::Fragment;
            CODI_CORE_ASSERT(false, "Unknown shader stage");
            return Shader::Type::None;
        }

        static std::string ShaderTypeToString(Shader::Type type) {
            switch (type) {
            case Shader::Type::Vertex: return "vertex";
            case Shader::Type::Fragment: return "fragment";
            }
            CODI_CORE_ASSERT(false, "Unknown shader stage");
            return "vertex";
        }

    } // namespace Utils

} // namespace Codi
