//
// Created by zhouzian on 2024/9/16.
//

#ifndef ADGBUFFER_RENDERSYSTEM_H
#define ADGBUFFER_RENDERSYSTEM_H
#include <Graphic/AdVKBuffer.h>
#include <Graphic/AdVKRenderPass.h>
#include <Render/AdRenderContext.h>

#include "Graphic/AdVKCommon.h"
#include "Render/AdMaterial.h"
#include "AdApplication.h"
namespace ade{
#define NUM_MATERIAL_BATCH              16
#define NUM_MATERIAL_BATCH_MAX          2048

    class AdVKPipelineLayout;
    class AdVKPipeline;
    class AdVKDescriptorSetLayout;
    class AdVKDescriptorPool;

    class AdGbufferRenderSystem {
    public:
        void OnInit(AdVKRenderPass *renderPass) ;
        void OnRender(VkCommandBuffer cmdBuffer) ;
        void OnDestroy();

        VkDescriptorSet GetFrameUboDescriptor() { return  mFrameUboDescSet; }
        std::shared_ptr<AdVKBuffer> GetFrameUboBuffer() { return mFrameUboBuffer; }
        std::shared_ptr<AdVKPipelineLayout> GetPipelineLayout() { return mPipelineLayout; }
    private:
        //void UpdateMaterialResourceDescSet(VkDescriptorSet descSet, AdSkyBoxComponent *skyBoxComp) ;
        std::shared_ptr<AdVKDescriptorSetLayout> mFrameUboDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialParamDescSetLayout;
        std::shared_ptr<AdVKDescriptorSetLayout> mMaterialResourceDescSetLayout;

        std::shared_ptr<AdVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<AdVKPipeline> mPipeline;

        std::shared_ptr<AdVKDescriptorPool> mDescriptorPool;
        std::shared_ptr<AdVKDescriptorPool> mMaterialDescriptorPool;

        VkDescriptorSet mFrameUboDescSet;
        VkDescriptorSet mMaterialUboDescSet;
        std::shared_ptr<AdVKBuffer> mFrameUboBuffer;
        std::shared_ptr<AdVKBuffer> mMaterialUboBuffer;



        FrameUbo mFrameUbo;
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



#endif //ADGBUFFER_RENDERSYSTEM_H
