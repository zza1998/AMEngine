#ifndef ADMATERIAL_H
#define ADMATERIAL_H

#include "AdTexture.h"
#include "AdSampler.h"
#include "AdGraphicContext.h"
#include "entt/core/type_info.hpp"
#include "ECS/Component/AdLightComponent.h"
#include "AdGeometryUtil.h"
namespace ade{
// -------------------------------------------------------------------------------------------------

    struct FrameUbo{
        glm::mat4  projMat{ 1.f };
        glm::mat4  viewMat{ 1.f };
        alignas(8) glm::ivec2 resolution;
        alignas(4) uint32_t frameId;
        alignas(4) float exposure{4.5};
        alignas(4) float gamma{2.2};
        alignas(4) float time;
    };

    // Shader Params
    struct TextureParam{
        bool enable {true};
        alignas(4) float uvRotation{ 0.0f };
        alignas(16) glm::vec4 uvTransform { 1.0f, 1.0f, 0.0f, 0.0f };   // x,y --> scale, z,w --> translation
    };

    struct LightUbo{
        //alignas(16) glm::vec3 light{ 0, -5, -5 };
        //alignas(16) glm::ivec3 lightCount{ 0, 0, 0 };
        //DirectLight directLight;
        PointLight pointLights[LIGHT_MAX_COUNT];
        //Spotlight spotlights[LIGHT_MAX_COUNT];
    };
    struct ModelPC{
        alignas(16) glm::mat4 modelMat;
        alignas(16) glm::mat3 normalMat;
    };

// -------------------------------------------------------------------------------------------------

    struct TextureView{
        AdTexture *texture = nullptr;
        AdSampler *sampler = nullptr;
        bool bEnable = true;
        glm::vec2 uvTranslation{ 0.f, 0.f };
        float uvRotation{ 0.f };
        glm::vec2 uvScale { 1.f, 1.f };

        bool IsValid() const {
            return bEnable && texture != nullptr && sampler != nullptr;
        }
    };

    class AdMaterial{
    public:
        AdMaterial(const AdMaterial&) = delete;
        AdMaterial &operator=(const AdMaterial&) = delete;

        static void UpdateTextureParams(const TextureView *textureView, TextureParam *param){
            param->enable = textureView->IsValid() && textureView->bEnable;
            param->uvRotation = textureView->uvRotation;
            param->uvTransform = { textureView->uvScale.x, textureView->uvScale.y, textureView->uvTranslation.x, textureView->uvTranslation.y };
        }

        int32_t GetIndex() const {
            return mIndex;
        }
        bool ShouldFlushParams() const { return bShouldFlushParams; }
        bool ShouldFlushResource() const { return bShouldFlushResource; }
        void FinishFlushParams() { bShouldFlushParams = false; }
        void FinishFlushResource() { bShouldFlushResource = false; }

        bool HasTexture(uint32_t id) const;
        const TextureView* GetTextureView(uint32_t id) const;
        void SetTextureView(uint32_t id, AdTexture *texture, AdSampler *sampler);
        void UpdateTextureViewEnable(uint32_t id, bool enable);
        void UpdateTextureViewUVTranslation(uint32_t id, const glm::vec2 &uvTranslation);
        void UpdateTextureViewUVRotation(uint32_t id, float uvRotation);
        void UpdateTextureViewUVScale(uint32_t id, const glm::vec2 &uvScale);
    protected:
        AdMaterial() = default;

        bool bShouldFlushParams = false;
        bool bShouldFlushResource = false;
    private:
        int32_t mIndex = -1;
        std::unordered_map<uint32_t, TextureView> mTextures;
        friend class AdMaterialFactory;
    };

    class AdMaterialFactory{
    public:
        AdMaterialFactory(const AdMaterialFactory&) = delete;
        AdMaterialFactory &operator=(const AdMaterialFactory&) = delete;

        static AdMaterialFactory* GetInstance(){
            return &s_MaterialFactory;
        }

        ~AdMaterialFactory() {
            mMaterials.clear();
        }

        template<typename T>
        size_t GetMaterialSize(){
            uint32_t typeId = entt::type_id<T>().hash();
            if(mMaterials.find(typeId) == mMaterials.end()){
                return 0;
            }
            return mMaterials[typeId].size();
        }

        template<typename T>
        T* CreateMaterial(){
            auto mat = std::make_shared<T>();
            uint32_t typeId = entt::type_id<T>().hash();

            uint32_t index = 0;
            if(mMaterials.find(typeId) == mMaterials.end()){
                mMaterials.insert({ typeId, { mat }});
            } else {
                index = mMaterials[typeId].size();
                mMaterials[typeId].push_back(mat);
            }
            mat->mIndex = index;
            return mat.get();
        }
    private:
        AdMaterialFactory() = default;

        static AdMaterialFactory s_MaterialFactory;

        std::unordered_map<uint32_t, std::vector<std::shared_ptr<AdMaterial>>> mMaterials;
    };
}

#endif