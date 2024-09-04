//
// Created by zhou on 2024/9/3.
//

#ifndef ADPHONGMATERIALCOMPONENT_H
#define ADPHONGMATERIALCOMPONENT_H

#include  "AdMaterialComponent.h"

namespace ade{

    struct FrameUbo{
        glm::mat4  projMat{ 1.f };
        glm::mat4  viewMat{ 1.f };
        alignas(8) glm::ivec2 resolution;
        alignas(4) uint32_t frameId;
        alignas(4) float time;
    };

    struct PhongMaterialUbo{
        alignas(16) glm::vec3 baseColor0;
        alignas(16) TextureParam textureParam0;
    };

    class AdPhongMaterial : public AdMaterial{
    public:
        const PhongMaterialUbo &GetParams() const { return mParams; }
        void SetBaseColor0(const glm::vec3 &color) {
            mParams.baseColor0 = color;
            bShouldFlushParams = true;
        }
    private:
        PhongMaterialUbo mParams{};
    };

    class AdPhongMaterialComponent : public AdMaterialComponent<AdPhongMaterial>{

    };
}



#endif //ADPHONGMATERIALCOMPONENT_H
