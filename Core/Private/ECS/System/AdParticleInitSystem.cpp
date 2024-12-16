//
// Created by zhou on 2024/12/3.
//

#include "ECS/System/AdParticleInitSystem.h"

#include <AdFileUtil.h>
#include <Graphic/AdVKComputePipeline.h>
#include <Graphic/AdVKDescriptorSet.h>
#include <Graphic/AdVKGraphicPipeline.h>

namespace ade {
    bool AdParticleInitSystem::initParticles() {
        AdVKDevice *device = GetDevice();

        //1 create pipeline:
        //---------------
        VkShaderModule computerShader;
        std::vector<char> content = ReadCharArrayFromFile(AD_RES_SHADER_DIR"particle_init.comp.spv");

        VkShaderModuleCreateInfo shaderModuleInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = static_cast<uint32_t>(content.size()),
            .pCode = reinterpret_cast<const uint32_t *>(content.data())
        };
        vkCreateShaderModule(device->GetHandle(), &shaderModuleInfo, nullptr, &computerShader);

        //2 create descriptor sets:
        //---------------
        {
            const std::vector<VkDescriptorSetLayoutBinding> bindings{
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                    .pImmutableSamplers = nullptr
                }
            };
            mDescSetLayout = std::make_shared<AdVKDescriptorSetLayout>(device, bindings);
        }
        std::vector<VkDescriptorPoolSize> poolSizes = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1
            }
        };
        mDescriptorPool = std::make_shared<AdVKDescriptorPool>(device, 10, poolSizes);
        mDescSets = mDescriptorPool->AllocateDescriptorSet(mDescSetLayout.get(), 1);

        VkPushConstantRange modelPC = {
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .offset = 0,
            .size = sizeof(ParticleGenParamsGPU)
        };
        ShaderLayout shaderLayout = {
            .descriptorSetLayouts = {
                mDescSetLayout->GetHandle()
            },
            .pushConstants = {modelPC}
        };
        mPipelineLayout = std::make_shared<AdVKPipelineLayout>(device, AD_RES_SHADER_DIR"particle_init.comp",
                                                               shaderLayout);

        mPipeline = std::make_shared<AdVkComputePipeline>(computerShader, mPipelineLayout->GetHandle(),
                                                          device->GetHandle());
        //3 run pipeline:
        //---------------

        ParticleGenParamsGPU params;
        params.numStars = DRAW_NUM_PARTICLES;
        params.maxRad = 3500.0f;
        params.bulgeRad = 1250.0f;
        params.angleOffset = 6.28f;
        params.eccentricity = 0.85f;
        params.baseHeight = 300.0f;
        params.height = 250.0f;
        params.minTemp = 3000.0f;
        params.maxTemp = 9000.0f;
        params.dustBaseTemp = 4000.0f;
        params.minStarOpacity = 0.1f;
        params.maxStarOpacity = 0.5f;
        params.minDustOpacity = 0.01f;
        params.maxDustOpacity = 0.05f;
        params.speed = 10.0f;
        uint32_t dynamicOffset = 0;
        VkCommandBuffer OnceCmdBuffer = device->CreateAndBeginOneCmdBuffer(false);
        vkCmdBindPipeline(OnceCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mPipeline->Get());
        vkCmdBindDescriptorSets(OnceCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mPipelineLayout->GetHandle(), 0, 1,
                                &mDescSets[0], 1, &dynamicOffset);
        vkCmdPushConstants(OnceCmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_COMPUTE_BIT, 0,
                           sizeof(ParticleGenParamsGPU), &params);
        vkCmdDispatch(OnceCmdBuffer, DRAW_NUM_PARTICLES / DRAW_PARTICLE_WORK_GROUP_SIZE, 1, 1);

        device->SubmitOneCmdBuffer(OnceCmdBuffer,false);
        vkDeviceWaitIdle(device->GetHandle());
        //4 cleanup:
        //---------------
        mDescriptorPool.reset();
        mDescSetLayout.reset();
        mPipelineLayout.reset();
        mPipeline.reset();
        return true;
    }

    AdParticleInitSystem::AdParticleInitSystem() {
    }

    AdParticleInitSystem::~AdParticleInitSystem() {
        mDescriptorPool.reset();
        mDescSetLayout.reset();
        mPipelineLayout.reset();
        mPipeline.reset();
    }
}
