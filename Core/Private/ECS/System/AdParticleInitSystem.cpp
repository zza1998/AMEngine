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
        std::vector<char> content = ReadCharArrayFromFile(AD_RES_SHADER_DIR"particle_init.comp");

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
        VkCommandBuffer OnceCmdBuffer = device->CreateAndBeginOneCmdBuffer(false);
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

        vkCmdBindPipeline(OnceCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mPipeline->Get());
        vkCmdBindDescriptorSets(OnceCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mPipelineLayout->GetHandle(), 0, 1,
                                &mDescSets[0], 1, &dynamicOffset);
        vkCmdPushConstants(OnceCmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_COMPUTE_BIT, 0,
                           sizeof(ParticleGenParamsGPU), &params);
        vkCmdDispatch(OnceCmdBuffer, DRAW_NUM_PARTICLES / DRAW_PARTICLE_WORK_GROUP_SIZE, 1, 1);

        device->SubmitOneCmdBuffer(OnceCmdBuffer);
        vkDeviceWaitIdle(device->GetHandle());
        //4 cleanup:
        //---------------
    }


    //    VKHcomputePipeline* pipeline;
    //	VKHdescriptorSets* descriptorSets;
    //
    //
    //	pipeline = vkh_compute_pipeline_create();
    //	if(!pipeline)
    //		return false;
    //
    //	uint64 computeCodeSize;
    //	uint32 *computeCode = vkh_load_spirv("assets/spirv/particle_generate.comp.spv", &computeCodeSize);
    //	VkShaderModule computeModule = vkh_create_shader_module(s->instance, computeCodeSize, computeCode);
    //	vkh_compute_pipeline_set_shader(pipeline, computeModule);
    //
    //	VkDescriptorSetLayoutBinding particleLayoutBinding = {};
    //	particleLayoutBinding.binding = 0;
    //	particleLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    //	particleLayoutBinding.descriptorCount = 1;
    //	particleLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    //	particleLayoutBinding.pImmutableSamplers = nullptr;
    //
    //	vkh_compute_pipeline_add_desc_set_binding(pipeline, particleLayoutBinding);
    //
    //	VkPushConstantRange pushConstant = {};
    //	pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    //	pushConstant.offset = 0;
    //	pushConstant.size = sizeof(ParticleGenParamsGPU);
    //
    //	vkh_compute_pipeline_add_push_constant(pipeline, pushConstant);
    //
    //	if(!vkh_compute_pipeline_generate(pipeline, s->instance))
    //		return false;
    //
    //	//2 create descriptor sets:
    //	//---------------
    //	descriptorSets = vkh_descriptor_sets_create(1);
    //	if(!descriptorSets)
    //		return false;
    //
    //	VkDescriptorBufferInfo particleBufferInfo = {};
    //	particleBufferInfo.buffer = s->particleBuffer;
    //	particleBufferInfo.offset = 0;
    //	particleBufferInfo.range = VK_WHOLE_SIZE;
    //
    //	vkh_descriptor_sets_add_buffers(descriptorSets, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    //		0, 0, 1, &particleBufferInfo);
    //
    //	if(!vkh_desctiptor_sets_generate(descriptorSets, s->instance, pipeline->descriptorLayout))
    //		return false;
    //
    //	//3 run pipeline:
    //	//---------------
    //	VkCommandBuffer commandBuf = vkh_start_single_time_command(s->instance);
    //
    //	ParticleGenParamsGPU params;
    //	params.numStars = DRAW_NUM_STARS;
    //	params.maxRad = 3500.0f;
    //	params.bulgeRad = 1250.0f;
    //	params.angleOffset = 6.28f;
    //	params.eccentricity = 0.85f;
    //	params.baseHeight = 300.0f;
    //	params.height = 250.0f;
    //	params.minTemp = 3000.0f;
    //	params.maxTemp = 9000.0f;
    //	params.dustBaseTemp = 4000.0f;
    //	params.minStarOpacity = 0.1f;
    //	params.maxStarOpacity = 0.5f;
    //	params.minDustOpacity = 0.01f;
    //	params.maxDustOpacity = 0.05f;
    //	params.speed = 10.0f;
    //
    //	uint32 dynamicOffset = 0;
    //	vkCmdBindPipeline(commandBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipeline);
    //	vkCmdBindDescriptorSets(commandBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->layout, 0, 1, &descriptorSets->sets[0], 1, &dynamicOffset);
    //	vkCmdPushConstants(commandBuf, pipeline->layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ParticleGenParamsGPU), &params);
    //	vkCmdDispatch(commandBuf, DRAW_NUM_PARTICLES / DRAW_PARTICLE_WORK_GROUP_SIZE, 1, 1);
    //
    //	vkh_end_single_time_command(s->instance, commandBuf);
    //
    //	vkDeviceWaitIdle(s->instance->device);
    //
    //	//4 cleanup:
    //	//---------------
    //	vkh_descriptor_sets_cleanup(descriptorSets, s->instance);
    //	vkh_descriptor_sets_destroy(descriptorSets);
    //
    //	vkh_compute_pipeline_cleanup(pipeline, s->instance);
    //	vkh_compute_pipeline_destroy(pipeline);
    //
    //	vkh_free_spirv(computeCode);
    //	vkh_destroy_shader_module(s->instance, computeModule);
    //
    //	return true;
}
