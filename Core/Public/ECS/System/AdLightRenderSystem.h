//
// Created by zhouzian on 2024/10/4.
//

#ifndef ADLIGHTRENDERSYSTEM_H
#define ADLIGHTRENDERSYSTEM_H

#include <Graphic/AdVKBuffer.h>
#include <Graphic/AdVKRenderPass.h>
#include <Render/AdRenderContext.h>

#include "Graphic/AdVKCommon.h"
#include "Render/AdMaterial.h"
#include "AdApplication.h"
#include "AdMaterialSystem.h"

namespace ade{
#define NUM_MATERIAL_BATCH              16
#define NUM_MATERIAL_BATCH_MAX          2048

    class AdVKPipelineLayout;
    class AdVKPipeline;
    class AdVKDescriptorSetLayout;
    class AdVKDescriptorPool;

    class AdLightRenderSystem : AdMaterialSystem{
    public:
        void OnInit(AdVKRenderPass *renderPass) override;
        void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) override;
        void OnDestroy() override;

    private:

        void UpdateLightUboDescSet();
        void UpdateGbufferUboDescSet(AdRenderTarget *renderTarget);
        void UpdateFrameUboDescSet(AdRenderTarget *renderTarget);
        std::shared_ptr<AdVKDescriptorSetLayout> mFrameUboDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialParamDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mLightDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialResourceDescSetLayout;

        std::shared_ptr<AdVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<AdVKPipeline> mPipeline;

        std::shared_ptr<AdVKDescriptorPool> mDescriptorPool;
        std::shared_ptr<AdVKDescriptorPool> mMaterialDescriptorPool;

        VkDescriptorSet mGbufferUboDescSet;
        VkDescriptorSet mLightUboDescSet;
        VkDescriptorSet mFrameUboDescSet;
        std::shared_ptr<AdVKBuffer> mGbufferUboBuffer;
        std::shared_ptr<AdVKBuffer> mLightUboBuffer;
        std::shared_ptr<AdVKBuffer> mFrameUboBuffer;

        FrameUbo mFrameUbo;
        LightUbo mLightUbo;
        uint32_t mLastDescriptorSetCount = 0;
        std::vector<VkDescriptorSet> mMaterialDescSets;
        std::vector<VkDescriptorSet> mMaterialResourceDescSets;
        std::vector<std::shared_ptr<AdVKBuffer>> mMaterialBuffers;


        AdVKDevice *GetDevice() const {
            ade::AdAppContext *appContext = AdApplication::GetAppContext();
            if(appContext){
                if(appContext->renderCxt){
                    return appContext->renderCxt->GetDevice();
                }
            }
            return nullptr;
        }
    };


}


#endif //ADLIGHTRENDERSYSTEM_H
