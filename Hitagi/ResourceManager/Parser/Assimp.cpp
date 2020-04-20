#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Assimp.hpp"

namespace Hitagi::Resource {

std::unique_ptr<Scene> AssimpParser::Parse(const Core::Buffer& buf) {
    std::unique_ptr<Scene> scene(new Scene);
    Assimp::Importer       importer;

    auto flag =
        aiPostProcessSteps::aiProcess_Triangulate |
        aiPostProcessSteps::aiProcess_CalcTangentSpace |
        aiPostProcessSteps::aiProcess_GenSmoothNormals |
        aiPostProcessSteps::aiProcess_JoinIdenticalVertices;

    const aiScene* _scene = importer.ReadFileFromMemory(buf.GetData(), buf.GetDataSize(), flag);

    if (!_scene) {
        std::cerr << "[AssimpParser] Can parse the scene." << std::endl;
        return nullptr;
    }

    auto createMesh = [&](const aiMesh* _mesh) -> std::shared_ptr<SceneObjectMesh> {
        auto mesh = std::make_shared<SceneObjectMesh>();
        // Set primitive type
        switch (_mesh->mPrimitiveTypes) {
            case aiPrimitiveType::aiPrimitiveType_LINE:
                mesh->SetPrimitiveType(PrimitiveType::LINE_LIST);
                break;
            case aiPrimitiveType::aiPrimitiveType_POINT:
                mesh->SetPrimitiveType(PrimitiveType::POINT_LIST);
                break;
            case aiPrimitiveType::aiPrimitiveType_POLYGON:
                mesh->SetPrimitiveType(PrimitiveType::POLYGON);
                break;
            case aiPrimitiveType::aiPrimitiveType_TRIANGLE:
                mesh->SetPrimitiveType(PrimitiveType::TRI_LIST);
                break;
            case aiPrimitiveType::_aiPrimitiveType_Force32Bit:
            default:
                std::cerr << "[AssimpParser] Unsupport Primitive Type" << std::endl;
        }

        // Read Position
        if (_mesh->HasPositions()) {
            std::vector<vec3f> pos(_mesh->mNumVertices);
            for (size_t i = 0; i < _mesh->mNumVertices; i++)
                pos[i] = vec3f(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);
            mesh->AddVertexArray(SceneObjectVertexArray("POSITION", 0, VertexDataType::FLOAT3, pos.data(), pos.size()));
        }

        // Read Normal
        if (_mesh->HasNormals()) {
            std::vector<vec3f> normal(_mesh->mNumVertices);
            for (size_t i = 0; i < _mesh->mNumVertices; i++)
                normal[i] = vec3f(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
            mesh->AddVertexArray(SceneObjectVertexArray("NORMAL", 0, VertexDataType::FLOAT3, normal.data(), normal.size()));
        }

        // Read Color
        for (size_t colorChannels = 0; colorChannels < _mesh->GetNumColorChannels(); colorChannels++) {
            if (_mesh->HasVertexColors(colorChannels)) {
                std::vector<vec4f> color(_mesh->mNumVertices);
                for (size_t i = 0; i < _mesh->mNumVertices; i++)
                    color[i] = vec4f(_mesh->mColors[colorChannels][i].r,
                                     _mesh->mColors[colorChannels][i].g,
                                     _mesh->mColors[colorChannels][i].b,
                                     _mesh->mColors[colorChannels][i].a);
                const auto attr = std::string("COLOR") + (colorChannels == 0 ? "" : std::to_string(colorChannels));
                mesh->AddVertexArray(SceneObjectVertexArray(attr, 0, VertexDataType::FLOAT4, color.data(), color.size()));
            }
        }

        // Read UV
        for (size_t UVChannel = 0; UVChannel < _mesh->GetNumUVChannels(); UVChannel++) {
            if (_mesh->HasTextureCoords(UVChannel)) {
                std::vector<vec2f> texcoord(_mesh->mNumVertices);
                for (size_t i = 0; i < _mesh->mNumVertices; i++)
                    texcoord[i] = vec2f(_mesh->mTextureCoords[UVChannel][i].x, _mesh->mTextureCoords[UVChannel][i].y);

                const auto attr = std::string("TEXCOORD") + (UVChannel == 0 ? "" : std::to_string(UVChannel));
                mesh->AddVertexArray(SceneObjectVertexArray(attr, 0, VertexDataType::FLOAT2, texcoord.data(), texcoord.size()));
            }
        }

        // Read Tangent and Bitangent
        if (_mesh->HasTangentsAndBitangents()) {
            std::vector<vec3f> tangent(_mesh->mNumVertices), bitangent(_mesh->mNumVertices);
            for (size_t i = 0; i < _mesh->mNumVertices; i++) {
                tangent[i]   = vec3f(_mesh->mTangents[i].x, _mesh->mTangents[i].y, _mesh->mTangents[i].z);
                bitangent[i] = vec3f(_mesh->mBitangents[i].x, _mesh->mBitangents[i].y, _mesh->mBitangents[i].z);
            }
            mesh->AddVertexArray(SceneObjectVertexArray("TANGENT", 0, VertexDataType::FLOAT3, tangent.data(), tangent.size()));
            mesh->AddVertexArray(SceneObjectVertexArray("BITANGENT", 0, VertexDataType::FLOAT3, bitangent.data(), bitangent.size()));
        }

        // Read Indices
        std::vector<int> indices;
        for (size_t face = 0; face < _mesh->mNumFaces; face++)
            for (size_t i = 0; i < _mesh->mFaces[face].mNumIndices; i++)
                indices.push_back(_mesh->mFaces[face].mIndices[i]);
        mesh->AddIndexArray(SceneObjectIndexArray(0, IndexDataType::INT32, indices.data(), indices.size()));

        auto materialRef = _scene->mMaterials[_mesh->mMaterialIndex]->GetName().C_Str();
        mesh->SetMaterial(scene->Materials[materialRef]);
        return mesh;
    };

    // process camera
    for (size_t i = 0; i < _scene->mNumCameras; i++) {
        const auto _camera           = _scene->mCameras[i];
        auto       perspectiveCamera = std::make_shared<SceneObjectPerspectiveCamera>(_camera->mHorizontalFOV);
        perspectiveCamera->SetParam("near", _camera->mClipPlaneNear);
        perspectiveCamera->SetParam("far", _camera->mClipPlaneFar);
        scene->Cameras[_camera->mName.C_Str()] = perspectiveCamera;
    }

    // process light
    for (size_t i = 0; i < _scene->mNumLights; i++) {
        const auto                        _light = _scene->mLights[i];
        std::shared_ptr<SceneObjectLight> light;
        switch (_light->mType) {
            case aiLightSourceType::aiLightSource_AMBIENT:
                std::cout << "[AssimpParser] Enigne unsupport light type now: AMBIENT" << std::endl;
                break;
            case aiLightSourceType::aiLightSource_AREA:
                std::cout << "[AssimpParser] Enigne unsupport light type now: AREA" << std::endl;
                break;
            case aiLightSourceType::aiLightSource_DIRECTIONAL:
                std::cout << "[AssimpParser] Enigne unsupport light type now: DIRECTIONAL" << std::endl;
                break;
            case aiLightSourceType::aiLightSource_POINT: {
                vec4f color     = normalize(vec4f(_light->mColorDiffuse.r, _light->mColorDiffuse.g, _light->mColorDiffuse.b, 1.0f));
                float intensity = _light->mColorDiffuse.r / color.r;
                light           = std::make_shared<SceneObjectPointLight>(color, intensity);
            } break;
            case aiLightSourceType::aiLightSource_SPOT: {
                vec4f diffuseColor(_light->mColorDiffuse.r, _light->mColorDiffuse.g, _light->mColorDiffuse.b, 1.0f);
                float intensity = _light->mColorDiffuse.r / diffuseColor.r;
                vec3f direction(_light->mDirection.x, _light->mDirection.y, _light->mDirection.z);
                light = std::make_shared<SceneObjectSpotLight>(
                    diffuseColor,
                    intensity,
                    direction,
                    _light->mAngleInnerCone,
                    _light->mAngleOuterCone);
            } break;
            case aiLightSourceType::aiLightSource_UNDEFINED:
                std::cout << "[AssimpParser] Enigne unsupport light type now: UNDEFINED" << std::endl;
                break;
            case aiLightSourceType::_aiLightSource_Force32Bit:
                std::cout << "[AssimpParser] Enigne unsupport light type now: Force32Bit" << std::endl;
                break;
            default:
                std::cerr << "[AssimpParser] Unknown light type." << std::endl;
                break;
        }
        scene->Lights[_light->mName.C_Str()] = light;
    }

    // process material
    for (size_t i = 0; i < _scene->mNumMaterials; i++) {
        auto       material  = std::make_shared<SceneObjectMaterial>();
        const auto _material = _scene->mMaterials[i];
        // set material name
        if (aiString name; AI_SUCCESS == _material->Get(AI_MATKEY_NAME, name))
            material->SetName(name.C_Str());
        // set material diffuse color
        if (aiColor3D diffuseColor; AI_SUCCESS == _material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor))
            material->SetColor("diffuse", vec4f(diffuseColor.r, diffuseColor.g, diffuseColor.b, 1.0f));
        // set material specular color
        if (aiColor3D specularColor; AI_SUCCESS == _material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor))
            material->SetColor("specular", vec4f(specularColor.r, specularColor.g, specularColor.b, 1.0f));
        // set material emission color
        if (aiColor3D emissiveColor; AI_SUCCESS == _material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor))
            material->SetColor("emission", vec4f(emissiveColor.r, emissiveColor.g, emissiveColor.b, 1.0f));
        // set material transparent color
        if (aiColor3D transparentColor; AI_SUCCESS == _material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparentColor))
            material->SetColor("transparency", vec4f(transparentColor.r, transparentColor.g, transparentColor.b, 1.0f));
        // set material shiness
        if (float shininess; AI_SUCCESS == _material->Get(AI_MATKEY_SHININESS, shininess))
            material->SetParam("specular_power", shininess);
        // set material opacity
        if (float opacity; AI_SUCCESS == _material->Get(AI_MATKEY_OPACITY, opacity))
            material->SetParam("opacity", opacity);

        // set diffuse texture
        // TODO: blend mutiple texture
        const std::unordered_map<std::string, aiTextureType> map = {
            {"diffuse", aiTextureType::aiTextureType_DIFFUSE},
            {"specular", aiTextureType::aiTextureType_SPECULAR},
            {"emission", aiTextureType::aiTextureType_EMISSIVE},
            {"opacity", aiTextureType::aiTextureType_OPACITY},
            // {"transparency", },
            {"normal", aiTextureType::aiTextureType_NORMALS},
        };
        for (auto&& [key1, key2] : map) {
            for (size_t i = 0; i < _material->GetTextureCount(key2); i++) {
                aiString path;
                if (AI_SUCCESS == _material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, i, &path))
                    material->SetTexture(key1, std::make_shared<SceneObjectTexture>(path.C_Str()));
                break;  // unsupport blend for now.
            }
        }

        scene->Materials[_material->GetName().C_Str()] = material;
    }

    // process meshes
    for (size_t i = 0; i < _scene->mNumMeshes; i++) {
        auto _mesh                          = _scene->mMeshes[i];
        scene->Meshes[_mesh->mName.C_Str()] = createMesh(_mesh);
    }

    auto getMatrix = [](const aiMatrix4x4& _mat) -> mat4f {
        mat4f ret;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                ret(i, j) = _mat[i][j];
        return ret;
    };

    auto createGeometry = [&](const aiNode* _node) -> std::shared_ptr<SceneObjectGeometry> {
        auto geometry = std::make_shared<SceneObjectGeometry>();
        for (size_t i = 0; i < _node->mNumMeshes; i++) {
            auto _mesh = _scene->mMeshes[_node->mMeshes[i]];
            geometry->AddMesh(scene->Meshes[_mesh->mName.C_Str()]);
        }
        return geometry;
    };

    std::function<std::shared_ptr<BaseSceneNode>(const aiNode*)>
        convert = [&](const aiNode* _node) -> std::shared_ptr<BaseSceneNode> {
        std::shared_ptr<BaseSceneNode> node;
        // The node is a geometry
        if (_node->mNumMeshes > 0) {
            scene->Geometries[_node->mName.C_Str()] = createGeometry(_node);

            auto geometryNode = std::make_shared<SceneGeometryNode>(_node->mName.C_Str());
            geometryNode->AddSceneObjectRef(_node->mName.C_Str());
            scene->GeometryNodes[_node->mName.C_Str()] = geometryNode;
            node                                       = geometryNode;
        }
        // The node is a camera
        else if (scene->Cameras.find(_node->mName.C_Str()) != scene->Cameras.end()) {
            auto cameraNode = std::make_shared<SceneCameraNode>(_node->mName.C_Str());
            cameraNode->AddSceneObjectRef(_node->mName.C_Str());
            scene->CameraNodes[_node->mName.C_Str()] = cameraNode;
            node                                     = cameraNode;
        }
        // The node is a light
        else if (scene->Lights.find(_node->mName.C_Str()) != scene->Lights.end()) {
            auto lightNode = std::make_shared<SceneLightNode>(_node->mName.C_Str());
            lightNode->AddSceneObjectRef(_node->mName.C_Str());
            scene->LightNodes[_node->mName.C_Str()] = lightNode;
            node                                    = lightNode;
        }
        // The node is empty
        else {
            node = std::make_shared<SceneEmptyNode>(_node->mName.C_Str());
        }

        // Add transform matrix
        node->AppendTransform(std::make_shared<SceneObjectTransform>(getMatrix(_node->mTransformation)));
        for (size_t i = 0; i < _node->mNumChildren; i++) {
            node->AppendChild(convert(_node->mChildren[i]));
        }
        return node;
    };

    scene->SceneGraph = convert(_scene->mRootNode);
    return scene;
}  // namespace Hitagi

}  // namespace Hitagi::Resource