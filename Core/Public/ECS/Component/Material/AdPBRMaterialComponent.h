//
// Created by zhou on 2024/9/14.
//

#ifndef ADPBRMATERIALCOMPONENT_H
#define ADPBRMATERIALCOMPONENT_H



#include  "AdMaterialComponent.h"

namespace ade{



    struct PBRMaterialUbo{
        alignas(4) float roughness;
        alignas(4) float metallic;
        alignas(16) glm::vec3 ambient;
        PBRMaterialUbo() = default;
        PBRMaterialUbo(float roughness, float metallic, glm::vec3 ambient) {
            this->roughness = roughness;
            this->metallic = metallic;
            this->ambient = ambient;
        };
        // metallic texture  roughness texture
    };


    class AdPBRMaterial : public AdMaterial{
    public:
        const PBRMaterialUbo &GetParams() const { return mParams; }
        const LightUbo &GetLight() const { return mLight; }
        void SetPBRMaterialUbo(const PBRMaterialUbo &ubo){mParams = ubo;}
    private:
        PBRMaterialUbo mParams{};
        LightUbo mLight{};
    };

    class AdPBRMaterialComponent : public AdMaterialComponent<AdPBRMaterial>{
    public:
        void OnDrawGui() override {

        };
    };
}




#endif //ADPBRMATERIALCOMPONENT_H
