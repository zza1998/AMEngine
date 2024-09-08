//
// Created by zhouzian on 2024/9/7.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <map>

#include "AdEngine.h"
#include "Render/AdMesh.h"
#include <assimp/scene.h>
#include <Render/AdMaterial.h>
#include <Render/AdTexture.h>

namespace ade {
    class AdModel{
    public:
        AdModel(const std::string &filePath);
        ~AdModel();

        [[nodiscard]] std::vector<AdMesh*> GetMeshes(uint32_t materialIndex) const;

        std::unordered_map<uint32_t,std::shared_ptr<AdTexture>> mMaterials;
        std::unordered_map<std::shared_ptr<AdMesh>,uint32_t> mMeshTextureMap;
    private:
        void ProcessNode(aiNode *node, const aiScene *scene);
        void ProcessMesh(aiMesh *mesh, const aiScene *scene);
        std::string mDirectory;
        std::vector<std::shared_ptr<AdMesh>> mMeshes;
        float maxX = -10.0,maxY=-10.0,minX=10.0,minY=10.0;
        bool gammaCorrection;
    };

}



#endif //MODELLOADER_H
