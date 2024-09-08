//
// Created by zhouzian on 2024/9/7.
//
#include "AdLog.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Loader/ModelLoader.h"

#include <ECS/Component/Material/AdPhongMaterialComponent.h>

namespace ade {
    AdModel::AdModel(const std::string &filePath) {
        LOG_D("Load Model: {0} : start", filePath);
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(filePath, aiProcess_Triangulate);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            LOG_E("ASSIMP::{0}:{1}", filePath, import.GetErrorString());
            return;
        }

        std::filesystem::path path(filePath);
        mDirectory = path.parent_path().string().append("/");

        ProcessNode(scene->mRootNode, scene);

        LOG_D("Load Model: {0} : end", filePath);

        import.FreeScene();
    }

    AdModel::~AdModel() {
        mMeshes.clear();
    }

    void AdModel::ProcessNode(aiNode *node, const aiScene *scene) {
        LOG_D("Node: {0}, Parent: {1}", node->mName.C_Str(), !node->mParent ? "Null" : node->mParent->mName.C_Str());
        for (int i = 0; i < node->mNumMeshes; i++) {
            ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene);
        }
        for (int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    void AdModel::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
        LOG_D("Mesh: {0}", mesh->mName.C_Str());

        std::vector<AdVertex> vertices;
        std::vector<uint32_t> indices;

        for (int i = 0; i < mesh->mNumVertices; i++) {
            AdVertex vertex{};
            vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

            aiVector3D *texcoord0 = mesh->mTextureCoords[0];
            if (texcoord0) {
                vertex.texcoord0 = {texcoord0[i].x, -texcoord0[i].y};
                maxX = std::max(texcoord0[i].x, maxX);
                maxY = std::max(texcoord0[i].y, maxY);
                minX = std::min(texcoord0[i].x, minX);
                minY = std::min(texcoord0[i].y, minY);
            }
            vertices.push_back(vertex);
        }

        for (int i = 0; i < mesh->mNumFaces; i++) {
            for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
                indices.push_back(mesh->mFaces[i].mIndices[j]);
            }
        }
        //Material
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        if(!mMaterials.contains(mesh->mMaterialIndex)) {
            // 后续可获取PBR材质系数
            aiString str;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            /*aiColor3D kd;
            material->Get(AI_MATKEY_COLOR_AMBIENT, kd);*/
            std::shared_ptr<ade::AdTexture>  tx = std::make_shared<ade::AdTexture>(mDirectory+str.C_Str());
            mMaterials[mesh->mMaterialIndex] = tx;
        }

        std::shared_ptr<AdMesh> adMesh = std::make_shared<AdMesh>(vertices, indices);
        mMeshes.push_back(adMesh);
        mMeshTextureMap[adMesh] = mesh->mMaterialIndex;
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<AdTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
        std::vector<AdTexture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) //检查储存在材质中(该类型)纹理的数量
        {
            aiString str;
            mat->GetTexture(type, i, &str); //获取每个纹理的文件位置，它会将结果储存在一个aiString中
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            /*for (unsigned int j = 0; j < mMaterials.size(); j++)
            {
                if (std::strcmp(mMaterials[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(mMaterials[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }*/
            if (!skip) {
                // if texture hasn't been loaded already, load it
                /*AdTexture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);//将会（用stb_image.h）加载一个纹理并返回该纹理的ID。
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);//在该容器中存储的只有id，类型和路径，对应特定的网络mesh
                textures_loaded.push_back(texture);  // 将其存储为整个模型加载的纹理，以确保我们不会不必要地加载重复的纹理。  */
            }
        }
        return textures;
    }


    std::vector<AdMesh *> AdModel::GetMeshes(uint32_t materialIndex) const {
        std::vector<AdMesh *> retMeshes;
        if (materialIndex <= 0) {
            for (auto mesh: mMeshes) {
                retMeshes.push_back(mesh.get());
            }
        }
        return retMeshes;
    }
}
