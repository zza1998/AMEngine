#ifndef ADMATERIALCOMPONENT_H
#define ADMATERIALCOMPONENT_H

#include "Render/AdMesh.h"
#include "Render/AdMaterial.h"
#include "ECS/AdComponent.h"
#include <Gui/Widget/AdWidget.h>
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

            auto it = std::find(mMaterials.begin(), mMaterials.end(), material);
            if(it!= mMaterials.end()){
                uint32_t index = std::distance(mMaterials.begin(), it);
                mMeshMaterials[index].push_back(meshIndex);
            } else {
                uint32_t index = mMaterials.size();
                mMaterials.push_back(material);
                mMeshMaterials.insert({ index, { meshIndex } });
            }
        }

        uint32_t GetMaterialCount() const {
            return mMaterials.size();
        }

        const std::unordered_map<uint32_t, std::vector<uint32_t>> &GetMeshMaterials() const {
            return mMeshMaterials;
        }

        AdMesh *GetMesh(uint32_t index) const {
            if(index < mMeshList.size()){
                return mMeshList[index];
            }
            return nullptr;
        }
        T* GetMeshMaterial(uint32_t index) const {
            if(index < mMeshList.size()) {
                return mMaterials[index];
            }
            return nullptr;
        }
    protected:
        std::vector<AdMesh*> mMeshList;
        std::vector<T*> mMaterials;
        std::unordered_map<uint32_t, std::vector<uint32_t>> mMeshMaterials;
    };
}

#endif