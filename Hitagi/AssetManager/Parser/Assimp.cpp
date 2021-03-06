#include "Assimp.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>

#include "HitagiMath.hpp"

namespace Hitagi::Asset {

Scene AssimpParser::Parse(const Core::Buffer& buf, const std::filesystem::path& scenePath) {
    auto logger = spdlog::get("AssetManager");
    if (buf.Empty()) {
        logger->warn("[Assimp] Parsing a empty buffer will return empty scene.");
        return Scene{};
    }

    auto             begin = std::chrono::high_resolution_clock::now();
    Scene            scene;
    Assimp::Importer importer;

    auto flag =
        aiPostProcessSteps::aiProcess_Triangulate |
        aiPostProcessSteps::aiProcess_CalcTangentSpace |
        aiPostProcessSteps::aiProcess_GenSmoothNormals |
        aiPostProcessSteps::aiProcess_JoinIdenticalVertices;

    const aiScene* _scene = importer.ReadFileFromMemory(buf.GetData(), buf.GetDataSize(), flag);
    if (!_scene) {
        logger->error("[Assimp] Can not parse the scene.");
        return scene;
    }
    auto end = std::chrono::high_resolution_clock::now();
    logger->info("[Assimp] Parsing costs {} ms.", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
    begin = std::chrono::high_resolution_clock::now();

    // process camera
    std::unordered_map<std::string_view, unsigned> cameraNameToIndex;
    for (size_t i = 0; i < _scene->mNumCameras; i++) {
        const auto _camera = _scene->mCameras[i];
        auto       perspectiveCamera =
            std::make_shared<SceneObjectCamera>(
                _camera->mAspect,
                _camera->mClipPlaneNear,
                _camera->mClipPlaneFar,
                _camera->mHorizontalFOV);

        scene.Cameras[_camera->mName.C_Str()]     = perspectiveCamera;
        cameraNameToIndex[_camera->mName.C_Str()] = i;
    }

    // process light
    for (size_t i = 0; i < _scene->mNumLights; i++) {
        const auto                        _light = _scene->mLights[i];
        std::shared_ptr<SceneObjectLight> light;
        switch (_light->mType) {
            case aiLightSourceType::aiLightSource_AMBIENT:
                std::cerr << "[AssimpParser] " << std::endl;
                logger->warn("[Assimp] Unsupport light type: AMBIEN");
                break;
            case aiLightSourceType::aiLightSource_AREA:
                logger->warn("[Assimp] Unsupport light type: AREA");
                break;
            case aiLightSourceType::aiLightSource_DIRECTIONAL:
                logger->warn("[Assimp] Unsupport light type: DIRECTIONAL");
                break;
            case aiLightSourceType::aiLightSource_POINT: {
                vec4f color(1.0f);
                color.rgb       = normalize(vec3f(_light->mColorDiffuse.r, _light->mColorDiffuse.g, _light->mColorDiffuse.b));
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
                logger->warn("[Assimp] Unsupport light type: UNDEFINED");
                break;
            case aiLightSourceType::_aiLightSource_Force32Bit:
                logger->warn("[Assimp] Unsupport light type: Force32Bit");
                break;
            default:
                logger->warn("[Assimp] Unknown light type.");
                break;
        }
        scene.Lights[_light->mName.C_Str()] = light;
    }

    // process material
    for (size_t i = 0; i < _scene->mNumMaterials; i++) {
        auto       material  = std::make_shared<SceneObjectMaterial>();
        const auto _material = _scene->mMaterials[i];
        // set material name
        if (aiString name; AI_SUCCESS == _material->Get(AI_MATKEY_NAME, name))
            material->SetName(name.C_Str());
        // set material diffuse color
        if (aiColor3D ambientColor; AI_SUCCESS == _material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor))
            material->SetColor("ambient", vec4f(ambientColor.r, ambientColor.g, ambientColor.b, 1.0f));
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
                aiString _path;
                if (AI_SUCCESS == _material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, i, &_path)) {
                    std::filesystem::path path(_path.C_Str());
                    if (path.is_relative()) path = (scenePath.parent_path() / path).lexically_normal();
                    material->SetTexture(key1, std::make_shared<SceneObjectTexture>(path));
                }
                break;  // unsupport blend for now.
            }
        }

        scene.Materials[_material->GetName().C_Str()] = material;
    }

    auto createMesh = [&](const aiMesh* _mesh) -> std::unique_ptr<SceneObjectMesh> {
        auto mesh = std::make_unique<SceneObjectMesh>();
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
                logger->error("[Assimp] Unsupport Primitive Type");
        }

        // Read Position
        if (_mesh->HasPositions()) {
            Core::Buffer positionBuffer(_mesh->mNumVertices * sizeof(vec3f));
            auto         position = reinterpret_cast<vec3f*>(positionBuffer.GetData());
            for (size_t i = 0; i < _mesh->mNumVertices; i++)
                position[i] = vec3f(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);
            mesh->AddVertexArray(SceneObjectVertexArray("POSITION", VertexDataType::FLOAT3, std::move(positionBuffer)));
        }

        // Read Normal
        if (_mesh->HasNormals()) {
            Core::Buffer normalBuffer(_mesh->mNumVertices * sizeof(vec3f));
            auto         normal = reinterpret_cast<vec3f*>(normalBuffer.GetData());
            for (size_t i = 0; i < _mesh->mNumVertices; i++)
                normal[i] = vec3f(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
            mesh->AddVertexArray(SceneObjectVertexArray("NORMAL", VertexDataType::FLOAT3, std::move(normalBuffer)));
        }

        // Read Color
        for (size_t colorChannels = 0; colorChannels < _mesh->GetNumColorChannels(); colorChannels++) {
            if (_mesh->HasVertexColors(colorChannels)) {
                Core::Buffer colorBuffer(_mesh->mNumVertices * sizeof(vec4f));
                auto         color = reinterpret_cast<vec4f*>(colorBuffer.GetData());
                for (size_t i = 0; i < _mesh->mNumVertices; i++)
                    color[i] = vec4f(_mesh->mColors[colorChannels][i].r,
                                     _mesh->mColors[colorChannels][i].g,
                                     _mesh->mColors[colorChannels][i].b,
                                     _mesh->mColors[colorChannels][i].a);
                const auto attr = std::string("COLOR") + (colorChannels == 0 ? "" : std::to_string(colorChannels));
                mesh->AddVertexArray(SceneObjectVertexArray(attr, VertexDataType::FLOAT4, std::move(colorBuffer)));
            }
        }

        // Read UV
        for (size_t UVChannel = 0; UVChannel < _mesh->GetNumUVChannels(); UVChannel++) {
            if (_mesh->HasTextureCoords(UVChannel)) {
                Core::Buffer texcoordBuffer(_mesh->mNumVertices * sizeof(vec2f));
                auto         texcoord = reinterpret_cast<vec2f*>(texcoordBuffer.GetData());
                for (size_t i = 0; i < _mesh->mNumVertices; i++)
                    texcoord[i] = vec2f(_mesh->mTextureCoords[UVChannel][i].x, _mesh->mTextureCoords[UVChannel][i].y);

                const auto attr = std::string("TEXCOORD") + (UVChannel == 0 ? "" : std::to_string(UVChannel));
                mesh->AddVertexArray(SceneObjectVertexArray(attr, VertexDataType::FLOAT2, std::move(texcoordBuffer)));
            }
        }

        // Read Tangent and Bitangent
        if (_mesh->HasTangentsAndBitangents()) {
            Core::Buffer tangentBuffer(_mesh->mNumVertices * sizeof(vec3f));
            auto         tangent = reinterpret_cast<vec3f*>(tangentBuffer.GetData());
            for (size_t i = 0; i < _mesh->mNumVertices; i++)
                tangent[i] = vec3f(_mesh->mTangents[i].x, _mesh->mTangents[i].y, _mesh->mTangents[i].z);
            mesh->AddVertexArray(SceneObjectVertexArray("TANGENT", VertexDataType::FLOAT3, std::move(tangentBuffer)));

            Core::Buffer bitangentBuffer(_mesh->mNumVertices * sizeof(vec3f));
            auto         bitangent = reinterpret_cast<vec3f*>(bitangentBuffer.GetData());
            for (size_t i = 0; i < _mesh->mNumVertices; i++)
                bitangent[i] = vec3f(_mesh->mBitangents[i].x, _mesh->mBitangents[i].y, _mesh->mBitangents[i].z);
            mesh->AddVertexArray(SceneObjectVertexArray("BITANGENT", VertexDataType::FLOAT3, std::move(bitangentBuffer)));
        }

        // Read Indices
        size_t indicesCount = 0;
        for (size_t face = 0; face < _mesh->mNumFaces; face++)
            indicesCount += _mesh->mFaces[face].mNumIndices;

        Core::Buffer indexBuffer(indicesCount * sizeof(int));
        auto         indices = reinterpret_cast<int*>(indexBuffer.GetData());
        for (size_t face = 0; face < _mesh->mNumFaces; face++)
            for (size_t i = 0; i < _mesh->mFaces[face].mNumIndices; i++)
                *indices++ = _mesh->mFaces[face].mIndices[i];  // assignment then increase

        mesh->AddIndexArray(SceneObjectIndexArray(IndexDataType::INT32, std::move(indexBuffer)));

        const std::string materialRef = _scene->mMaterials[_mesh->mMaterialIndex]->GetName().C_Str();
        mesh->SetMaterial(scene.Materials.at(materialRef));
        return mesh;
    };

    auto getMatrix = [](const aiMatrix4x4& _mat) -> mat4f {
        mat4f ret;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                ret[i][j] = _mat[i][j];
        return ret;
    };

    auto createGeometry = [&](const aiNode* _node) -> std::shared_ptr<SceneObjectGeometry> {
        auto geometry = std::make_shared<SceneObjectGeometry>();
        for (size_t i = 0; i < _node->mNumMeshes; i++) {
            auto _mesh = _scene->mMeshes[_node->mMeshes[i]];
            geometry->AddMesh(createMesh(_mesh));
        }
        return geometry;
    };

    std::function<std::shared_ptr<BaseSceneNode>(const aiNode*)>
        convert = [&](const aiNode* _node) -> std::shared_ptr<BaseSceneNode> {
        std::shared_ptr<BaseSceneNode> node;
        const std::string              name(_node->mName.C_Str());
        // The node is a geometry
        if (_node->mNumMeshes > 0) {
            scene.Geometries[name] = createGeometry(_node);

            auto geometryNode = std::make_shared<SceneGeometryNode>(name);
            geometryNode->AddSceneObjectRef(scene.GetGeometry(name));
            scene.GeometryNodes[name] = geometryNode;
            node                      = geometryNode;
        }
        // The node is a camera
        else if (scene.Cameras.find(name) != scene.Cameras.end()) {
            auto& _camera = _scene->mCameras[cameraNameToIndex[name]];
            // move space infomation to camera node
            auto cameraNode = std::make_shared<SceneCameraNode>(
                name,
                vec3f(_camera->mPosition.x, _camera->mPosition.y, _camera->mPosition.z),
                vec3f(_camera->mUp.x, _camera->mUp.y, _camera->mUp.z),
                vec3f(_camera->mLookAt.x, _camera->mLookAt.y, _camera->mLookAt.z));
            cameraNode->AddSceneObjectRef(scene.GetCamera(name));

            scene.CameraNodes[name] = cameraNode;
            node                    = cameraNode;
        }
        // The node is a light
        else if (scene.Lights.find(name) != scene.Lights.end()) {
            auto lightNode = std::make_shared<SceneLightNode>(name);
            lightNode->AddSceneObjectRef(scene.GetLight(name));
            scene.LightNodes[name] = lightNode;
            node                   = lightNode;
        }
        // The node is empty
        else {
            node = std::make_shared<SceneEmptyNode>(name);
        }

        // Add transform matrix
        node->AppendTransform(std::make_shared<SceneObjectTransform>(getMatrix(_node->mTransformation)));
        for (size_t i = 0; i < _node->mNumChildren; i++) {
            node->AppendChild(convert(_node->mChildren[i]));
        }
        return node;
    };

    scene.SceneGraph = convert(_scene->mRootNode);

    end = std::chrono::high_resolution_clock::now();
    logger->info("[Assimp] Processing costs {} ms.", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
    return scene;
}

}  // namespace Hitagi::Asset