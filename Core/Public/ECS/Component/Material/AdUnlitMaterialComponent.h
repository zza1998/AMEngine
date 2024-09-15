#ifndef ADUNLITMATERIALCOMPONENT_H
#define ADUNLITMATERIALCOMPONENT_H

#include "AdMaterialComponent.h"

namespace ade{
    enum UnlitMaterialTexture{
        UNLIT_MAT_BASE_COLOR_0,
        UNLIT_MAT_BASE_COLOR_1
    };

    struct UnlitMaterialUbo{
        alignas(16) glm::vec3 baseColor0;
        alignas(16) glm::vec3 baseColor1;
        alignas(4) float mixValue;
        alignas(16) TextureParam textureParam0;
        alignas(16) TextureParam textureParam1;
    };

    class AdUnlitMaterial : public AdMaterial{
    public:
        const UnlitMaterialUbo &GetParams() const { return mParams; }
        const glm::vec3 &GetBaseColor0() const { return mParams.baseColor0; }
        const glm::vec3 &GetBaseColor1() const { return mParams.baseColor1; }
        float GetMixValue() const { return mParams.mixValue; }

        void SetBaseColor0(const glm::vec3 &color) {
            mParams.baseColor0 = color;
            bShouldFlushParams = true;
        }

        void SetBaseColor1(const glm::vec3 &color) {
            mParams.baseColor1 = color;
            bShouldFlushParams = true;
        }

        void SetMixValue(float mixValue) {
            mParams.mixValue = mixValue;
            bShouldFlushParams = true;
        }
    private:
        UnlitMaterialUbo mParams{};
    };

    class AdUnlitMaterialComponent : public AdMaterialComponent<AdUnlitMaterial>{

    };
}

#endif