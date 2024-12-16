//
// Created by zhou on 2024/12/3.
//

#ifndef ADPARTICLEINITSYSTEM_H
#define ADPARTICLEINITSYSTEM_H

#include <AdApplication.h>
#include <Render/AdRenderContext.h>

#include "Graphic/AdVKCommon.h"

#define DRAW_NUM_PARTICLES 20000.0
#define DRAW_PARTICLE_WORK_GROUP_SIZE 32
namespace ade{

    class AdVKPipelineLayout;
    class AdVkComputePipeline;
    class AdVKDescriptorSetLayout;
    class AdVKDescriptorPool;

    struct ParticleGenParamsGPU
    {
        int32_t numStars;

        float maxRad;
        float bulgeRad;

        float angleOffset;
        float eccentricity;

        float baseHeight;
        float height;

        float minTemp;
        float maxTemp;
        float dustBaseTemp;

        float minStarOpacity;
        float maxStarOpacity;

        float minDustOpacity;
        float maxDustOpacity;

        float speed;
    };

    class AdParticleInitSystem {
       public:

         bool initParticles();
        AdParticleInitSystem();
        ~AdParticleInitSystem();
        AdVKDevice *GetDevice() const {
            ade::AdAppContext *appContext = AdApplication::GetAppContext();
            if(appContext){
                if(appContext->renderCxt){
                    return appContext->renderCxt->GetDevice();
                }
            }
            return nullptr;
        }
        std::shared_ptr<AdVKDescriptorPool> mDescriptorPool;
        std::vector<VkDescriptorSet> mDescSets;
        std::shared_ptr<AdVKDescriptorSetLayout> mDescSetLayout;
        std::shared_ptr<AdVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<AdVkComputePipeline> mPipeline;
    private:

    };



}



#endif //ADPARTICLEINITSYSTEM_H
