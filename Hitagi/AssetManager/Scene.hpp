#pragma once

#include "SceneNode.hpp"

namespace Hitagi::Asset {

class Scene {
private:
    std::shared_ptr<SceneObjectMaterial> m_DefaultMaterial;

public:
    std::shared_ptr<BaseSceneNode>                                      SceneGraph;
    std::unordered_map<std::string, std::shared_ptr<SceneCameraNode>>   CameraNodes;
    std::unordered_map<std::string, std::shared_ptr<SceneLightNode>>    LightNodes;
    std::unordered_map<std::string, std::shared_ptr<SceneGeometryNode>> GeometryNodes;

    std::unordered_map<std::string, std::shared_ptr<SceneObjectCamera>>   Cameras;
    std::unordered_map<std::string, std::shared_ptr<SceneObjectLight>>    Lights;
    std::unordered_map<std::string, std::shared_ptr<SceneObjectMaterial>> Materials;
    std::unordered_map<std::string, std::shared_ptr<SceneObjectGeometry>> Geometries;

public:
    Scene() {
        m_DefaultMaterial = std::make_shared<SceneObjectMaterial>("default");
        SceneGraph        = std::make_shared<BaseSceneNode>("default");
    }
    Scene(std::string_view scene_name) : SceneGraph(std::make_shared<BaseSceneNode>(scene_name)) {}
    ~Scene() = default;

    std::vector<std::reference_wrapper<SceneGeometryNode>> GetGeometries() const;

    std::shared_ptr<SceneObjectCamera>   GetCamera(const std::string& key) const;
    std::shared_ptr<SceneObjectLight>    GetLight(const std::string& key) const;
    std::shared_ptr<SceneObjectGeometry> GetGeometry(const std::string& key) const;
    std::shared_ptr<SceneObjectMaterial> GetMaterial(const std::string& key) const;

    std::shared_ptr<SceneCameraNode> GetFirstCameraNode() const;
    std::shared_ptr<SceneLightNode>  GetFirstLightNode() const;

    void LoadResource();
};
}  // namespace Hitagi::Asset