//
// Created by zhou on 2024/9/14.
//

#ifndef ADPBRMATERIALSYSTEM_H
#define ADPBRMATERIALSYSTEM_H


#include "AdGbufferRenderSystem.h"
#include "ECS/System/AdMaterialSystem.h"
#include "ECS/Component/Material/AdPBRMaterialComponent.h"
#include "AdGeometryUtil.h"

namespace ade{
#define NUM_MATERIAL_BATCH              16
#define NUM_MATERIAL_BATCH_MAX          2048

    class AdVKPipelineLayout;
    class AdVKPipeline;
    class AdVKDescriptorSetLayout;
    class AdVKDescriptorPool;

    class AdPBRMaterialSystem : public AdMaterialSystem {
    public:
        void OnInit(AdVKRenderPass *renderPass) override;
        void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) override;
        void OnDestroy() override;
        void SetGbufferSystem(std::shared_ptr<AdGbufferRenderSystem> gbufferRender) override { mGbufferRender = gbufferRender; };
    private:
        void ReCreateMaterialDescPool(uint32_t materialCount);
        void UpdateFrameUboDescSet(AdRenderTarget *renderTarget,VkDescriptorSet mFrameUboDescSet,std::shared_ptr<AdVKBuffer> mFrameUboBuffer);
        void UpdateLightUboDescSet();
        void UpdateMaterialParamsDescSet(VkDescriptorSet descSet, AdPBRMaterial *material);
        void UpdateMaterialResourceDescSet(VkDescriptorSet descSet, AdPBRMaterial *material);

        std::shared_ptr<AdVKDescriptorSetLayout> mFrameUboDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialParamDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialResourceDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mLightDescSetLayout;

        std::shared_ptr<AdGbufferRenderSystem> mGbufferRender;
        //std::shared_ptr<AdVKPipelineLayout> mPipelineLayout;
        //std::shared_ptr<AdVKPipeline> mPipeline;

        std::shared_ptr<AdVKDescriptorPool> mDescriptorPool;
        std::shared_ptr<AdVKDescriptorPool> mMaterialDescriptorPool;

        FrameUbo mFrameUbo;
        uint32_t mLastDescriptorSetCount = 0;
        std::vector<VkDescriptorSet> mMaterialDescSets;
        std::vector<VkDescriptorSet> mMaterialResourceDescSets;
        std::vector<std::shared_ptr<AdVKBuffer>> mMaterialBuffers;
    };


}





#endif //ADPBRMATERIALSYSTEM_H
