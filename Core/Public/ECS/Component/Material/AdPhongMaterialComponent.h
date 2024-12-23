//
// Created by zhou on 2024/9/3.
//

#ifndef ADPHONGMATERIALCOMPONENT_H
#define ADPHONGMATERIALCOMPONENT_H

#include <Gui/Widget/AdWidget.h>

#include  "AdMaterialComponent.h"

namespace ade{


    struct PhongMaterialUbo{
        alignas(16) glm::vec3 baseColor0;
        alignas(16) glm::vec3 specular{ 0.2f, 0.2f, 0.2f };
        alignas(16) TextureParam textureParam0;
        alignas(4) float shininess{ 2.f };
    };



    struct PhongLightMaterialParams{
        alignas(16) glm::vec3 ambient{ 0.2f, 0.2f, 0.2f };
        alignas(16) glm::vec3 diffuse{ 0.8f, 0.8f, 0.8f };
        alignas(16) glm::vec3 specular{ 1.f, 1.f, 1.f };
        alignas(4) float shininess{ 32.f };
        glm::mat4 diffuseTextureUvTransform{ 1.f };
        glm::mat4 specularTextureUvTransform{ 1.f };
        alignas(16) glm::ivec3 textureSelect{ 0, 0, 0 };  // x->ambient, y->diffuse, z->specular
    };

    class AdPhongMaterial : public AdMaterial{
    public:
        const PhongMaterialUbo &GetParams() const { return mParams; }
        void SetBaseColor0(const glm::vec3 &color) {
            mParams.baseColor0 = color;
            bShouldFlushParams = true;
        }
        void SetTextureParam0(const TextureParam &param) {
            mParams.textureParam0 = param;
            bShouldFlushParams = true;
        }
    private:
        PhongMaterialUbo mParams{};
    };

    class AdPhongMaterialComponent : public AdMaterialComponent<AdPhongMaterial>{
    public:

        void OnDrawGui() override;
        REG_COMPONENT(AdPhongMaterialComponent)
    };
}



#endif //ADPHONGMATERIALCOMPONENT_H
