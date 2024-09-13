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
        alignas(16) glm::vec3 specular{ 0.5f, 0.5f, 0.5f };
        alignas(16) TextureParam textureParam0;
        alignas(4) float shininess{ 2.f };
    };

    struct LightUbo{
        alignas(16) glm::vec3 light{ 0, -5, -5 };
        /*alignas(16) glm::ivec3 lightCount{ 0, 0, 0 };
        DirectLight directLight;
        PointLight pointLights[LIGHT_MAX_COUNT];
        Spotlight spotlights[LIGHT_MAX_COUNT];*/
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
        const LightUbo &GetLight() const { return mLight; }
        void SetBaseColor0(const glm::vec3 &color) {
            mParams.baseColor0 = color;
            bShouldFlushParams = true;
        }
        void SetTextureParam0(const TextureParam &param) {
            mParams.textureParam0 = param;
            bShouldFlushParams = true;
        }
        void SetLightParam0(const glm::vec3 &light) {
            mLight.light = light;
        }
    private:
        PhongMaterialUbo mParams{};
        LightUbo mLight{};
    };

    class AdPhongMaterialComponent : public AdMaterialComponent<AdPhongMaterial>{

    };
}



#endif //ADPHONGMATERIALCOMPONENT_H
