//
// Created by zhou on 2024/9/3.
//

#ifndef ADPHONGMATERIALSYSTEM_H
#define ADPHONGMATERIALSYSTEM_H
#include "ECS/System/AdMaterialSystem.h"
#include "ECS/Component/Material/AdPhongMaterialComponent.h"
#include "AdGeometryUtil.h"

namespace ade{
#define NUM_MATERIAL_BATCH              16
#define NUM_MATERIAL_BATCH_MAX          2048

    class AdVKPipelineLayout;
    class AdVKPipeline;
    class AdVKDescriptorSetLayout;
    class AdVKDescriptorPool;

    class AdPhongMaterialSystem : public AdMaterialSystem {
    public:
        void OnInit(AdVKRenderPass *renderPass) override;
        void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) override;
        void OnDestroy() override;
    private:
        void ReCreateMaterialDescPool(uint32_t materialCount);
        void UpdateFrameUboDescSet(AdRenderTarget *renderTarget);
        void UpdateLightUboDescSet();
        void UpdateMaterialParamsDescSet(VkDescriptorSet descSet, AdPhongMaterial *material);
        void UpdateMaterialResourceDescSet(VkDescriptorSet descSet, AdPhongMaterial *material);

        std::shared_ptr<AdVKDescriptorSetLayout> mFrameUboDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialParamDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialResourceDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mLightDescSetLayout;

        std::shared_ptr<AdVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<AdVKPipeline> mPipeline;

        std::shared_ptr<AdVKDescriptorPool> mDescriptorPool;
        std::shared_ptr<AdVKDescriptorPool> mMaterialDescriptorPool;

        VkDescriptorSet mFrameUboDescSet;
        VkDescriptorSet mLightUboDescSet;
        std::shared_ptr<AdVKBuffer> mFrameUboBuffer;
        std::shared_ptr<AdVKBuffer> mLightUboBuffer;

        LightUbo mLightUbo;
        FrameUbo mFrameUbo;
        uint32_t mLastDescriptorSetCount = 0;
        std::vector<VkDescriptorSet> mMaterialDescSets;
        std::vector<VkDescriptorSet> mMaterialResourceDescSets;
        std::vector<std::shared_ptr<AdVKBuffer>> mMaterialBuffers;
    };


}



#endif //ADPHONGMATERIALSYSTEM_H
