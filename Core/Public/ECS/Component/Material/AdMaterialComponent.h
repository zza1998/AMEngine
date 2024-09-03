#ifndef ADMATERIALCOMPONENT_H
#define ADMATERIALCOMPONENT_H

#include "Render/AdMesh.h"
#include "Render/AdMaterial.h"
#include "ECS/AdComponent.h"

namespace ade{
    template<typename T>
    class AdMaterialComponent : public AdComponent{
    public:
        void AddMesh(AdMesh *mesh, T *material = nullptr){
            if(!mesh){
                return;
            }
            uint32_t meshIndex = mMeshList.size();
            mMeshList.push_back(mesh);

            if(mMeshMaterials.find(material) != mMeshMaterials.end()){
                mMeshMaterials[material].push_back(meshIndex);
            } else {
                mMeshMaterials.insert({ material, { meshIndex } });
            }
        }

        uint32_t GetMaterialCount() const {
            return mMeshMaterials.size();
        }

        const std::unordered_map<T*, std::vector<uint32_t>> &GetMeshMaterials() const {
            return mMeshMaterials;
        }

        AdMesh *GetMesh(uint32_t index) const {
            if(index < mMeshList.size()){
                return mMeshList[index];
            }
            return nullptr;
        }
    private:
        std::vector<AdMesh*> mMeshList;
        std::unordered_map<T*, std::vector<uint32_t>> mMeshMaterials;
    };
}

#endif