//
// Created by zhou on 2024/9/25.
//

#include "ECS\Component\Material\AdPBRMaterialComponent.h"

namespace ade {


    void AdPBRMaterialComponent::OnDrawGui(){

        static float f1 = 1.0;
        static float f2 = 1.0;
        for (auto item : mMaterials) {
            AdPBRMaterial* m = item;
            if(AdWidget::DrawOneFloat("roughness",&m->mParams.roughness,0.005,0.0,1.0)) {
                m->MarkDirty();
            }
            if(AdWidget::DrawOneFloat("metallic",&m->mParams.metallic,0.005,0.0,1.0)) {
                m->MarkDirty();
            }
        }



    }

}
