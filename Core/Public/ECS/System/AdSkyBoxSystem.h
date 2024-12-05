//
// Created by zhouzian on 2024/9/16.
//

#ifndef ADSKYBOXSYSTEM_H
#define ADSKYBOXSYSTEM_H
#include <Graphic/AdVKBuffer.h>

#include "ECS/System/AdMaterialSystem.h"
#include "AdFileUtil.h"
#include "Render/AdMaterial.h"
#include "ECS/Component/AdSkyBoxComponent.h"
namespace ade{
#define NUM_MATERIAL_BATCH              16
#define NUM_MATERIAL_BATCH_MAX          2048

    class AdVKPipelineLayout;
    class AdVKPipeline;
    class AdVKDescriptorSetLayout;
    class AdVKDescriptorPool;

    class AdSkyBoxSystem : public  AdMaterialSystem{
    public:
        void OnInit(AdVKRenderPass *renderPass) ;
        void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) ;
        void OnDestroy();
    private:
        void ReCreateMaterialDescPool(uint32_t materialCount);
        void UpdateFrameUboDescSet(AdRenderTarget *renderTarget);
        void UpdateMaterialResourceDescSet(VkDescriptorSet descSet, AdSkyBoxComponent *skyBoxComp) ;
        std::shared_ptr<AdVKDescriptorSetLayout> mFrameUboDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialParamDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialResourceDescSetLayout;

        std::shared_ptr<AdVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<AdVKPipeline> mPipeline;

        std::shared_ptr<AdVKDescriptorPool> mDescriptorPool;
        std::shared_ptr<AdVKDescriptorPool> mMaterialDescriptorPool;

        VkDescriptorSet mFrameUboDescSet;
        VkDescriptorSet mLightUboDescSet;
        std::shared_ptr<AdVKBuffer> mFrameUboBuffer;



        FrameUbo mFrameUbo;
        uint32_t mLastDescriptorSetCount = 0;
        std::vector<VkDescriptorSet> mMaterialDescSets;
        std::vector<VkDescriptorSet> mMaterialResourceDescSets;
        std::vector<std::shared_ptr<AdVKBuffer>> mMaterialBuffers;
    };


}



#endif //ADSKYBOXSYSTEM_H
