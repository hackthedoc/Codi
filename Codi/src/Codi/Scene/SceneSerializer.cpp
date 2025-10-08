#include "cdpch.h"
#include "SceneSerializer.h"

#include "Codi/Scene/Component.h"

#include <yaml-cpp/yaml.h>

namespace YAML {

template<>
struct convert<glm::vec3> {
    static Node encode(const glm::vec3& rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs) {
        if (!node.IsSequence() || node.size() != 3) return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();

        return true;
    }
};

template<>
struct convert<glm::vec4> {
    static Node encode(const glm::vec4& rhs) {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        return node;
    }

    static bool decode(const Node& node, glm::vec4& rhs) {
        if (!node.IsSequence() || node.size() != 4) return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();

        return true;
    }
};

} // namespace YAML

namespace Codi {

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
    return out;
}

SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : _scene(scene) {}

SceneSerializer::~SceneSerializer() {}

static void SerializeEntity(YAML::Emitter& out, Entity entity) {
    out << YAML::BeginMap; // Entity
    out << YAML::Key << "Entity" << YAML::Value << "1212515925025025"; // TODO: entity ID
    
    if (entity.hasComponent<TagComponent>()) {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap; // TagComponent
        TagComponent& tag = entity.getComponent<TagComponent>();
        
        out << YAML::Key << "Tag" << YAML::Value << tag.tag;

        out << YAML::EndMap; // TagComponent
    }
    
    if (entity.hasComponent<TransformComponent>()) {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // TransformComponent
        TransformComponent& transform = entity.getComponent<TransformComponent>();

        out << YAML::Key << "Translation" << YAML::Value << transform.translation;
        out << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
        out << YAML::Key << "Scale" << YAML::Value << transform.scale;

        out << YAML::EndMap; // TransformComponent
    }
    
    if (entity.hasComponent<CameraComponent>()) {
        out << YAML::Key << "CameraComponent";
        out << YAML::BeginMap; // CameraComponent
        CameraComponent& camera = entity.getComponent<CameraComponent>();
        
        out << YAML::Key << "Camera";
        out << YAML::BeginMap; // Camera

        out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.camera.getProjectionType();
        
        out << YAML::Key << "PerspectiveVerticalFOV" << YAML::Value << camera.camera.getPerspectiveVerticalFOV();
        out << YAML::Key << "PerspectiveNearClip" << YAML::Value << camera.camera.getPerspectiveNearClip();
        out << YAML::Key << "PerspectiveFarClip" << YAML::Value << camera.camera.getPerspectiveFarClip();

        out << YAML::Key << "OrthographicSize" << YAML::Value << camera.camera.getOrthographicSize();
        out << YAML::Key << "OrthographicNearClip" << YAML::Value << camera.camera.getOrthographicNearClip();
        out << YAML::Key << "OrthographicFarClip" << YAML::Value << camera.camera.getOrthographicFarClip();

        out << YAML::EndMap; // Camera

        out << YAML::Key << "Primary" << YAML::Value << camera.primary;
        out << YAML::Key << "FixedAspectRatio" << YAML::Value << camera.fixedAspectRatio;

        out << YAML::EndMap; // CameraComponent
    }
    
    if (entity.hasComponent<SpriteRendererComponent>()) {
        out << YAML::Key << "SpriteRendererComponent";
        out << YAML::BeginMap; // SpriteRendererComponent
        SpriteRendererComponent& sprite = entity.getComponent<SpriteRendererComponent>();
        
        out << YAML::Key << "Color" << YAML::Value << sprite.color;

        out << YAML::EndMap; // SpriteRendererComponent
    }

    out << YAML::EndMap; // Entity
}

void SceneSerializer::serializeText(const std::string& filepath) {
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    _scene->_registry.view<TagComponent>().each([&](entt::entity entityID, TagComponent& tc) {
        const Entity entity{entityID, _scene.get()};
        if (!entity) return;
        SerializeEntity(out, entity);
    });
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream outfile(filepath);
    outfile << out.c_str();
    outfile.close();
}

void SceneSerializer::serializeBinary(const std::string& filepath) {
    CODI_CORE_ASSERT(false, "SceneSerializer::serializeBinary not yet implemented!");
}

bool SceneSerializer::deserializeText(const std::string& filepath) {
    std::ifstream infile(filepath);
    std::stringstream stream;
    stream << infile.rdbuf();
    
    YAML::Node data = YAML::Load(stream.str());
    if (!data["Scene"]) return false;

    const std::string sceneName = data["Scene"].as<std::string>();
    CODI_CORE_TRACE("Deserializing scene '{0}'", sceneName);

    auto entities = data["Entities"];
    if (entities) for (auto entity : entities) {
        uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO: entitie IDs

        std::string name;
        const auto& tagComponent = entity["TagComponent"];
        if (tagComponent) name = tagComponent["Tag"].as<std::string>();

        CODI_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

        Entity deserializedEntity = _scene->createEntity(name);

        const auto& transformComponent = entity["TransformComponent"];
        if (transformComponent) {
            TransformComponent& tc = deserializedEntity.getOrCreateComponent<TransformComponent>();
            tc.translation = transformComponent["Translation"].as<glm::vec3>();
            tc.rotation= transformComponent["Rotation"].as<glm::vec3>();
            tc.scale = transformComponent["Scale"].as<glm::vec3>();
        }

        const auto& cameraComponent = entity["CameraComponent"];
        if (cameraComponent) {
            CameraComponent& cc = deserializedEntity.getOrCreateComponent<CameraComponent>();
            
            auto& cameraProps = cameraComponent["Camera"];
            cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

            cc.camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveVerticalFOV"].as<float>());
            cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNearClip"].as<float>());
            cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFarClip"].as<float>());

            cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
            cc.camera.setOrthographicNearClip(cameraProps["OrthographicNearClip"].as<float>());
            cc.camera.setOrthographicFarClip(cameraProps["OrthographicFarClip"].as<float>());

            cc.primary = cameraComponent["Primary"].as<bool>();
            cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
        }

        const auto& spriteRendererComponent = entity["SpriteRendererComponent"];
        if (spriteRendererComponent) {
            SpriteRendererComponent& src = deserializedEntity.getOrCreateComponent<SpriteRendererComponent>();
            src.color = spriteRendererComponent["Color"].as<glm::vec4>();
        }
    }

    return true;
}

bool SceneSerializer::deserializeBinary(const std::string& filepath) {
    CODI_CORE_ASSERT(false, "SceneSerializer::deserializeBinary not yet implemented!");
    return false;
}

} // namespace Codi

