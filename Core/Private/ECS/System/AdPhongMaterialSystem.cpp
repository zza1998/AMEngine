//
// Created by zhou on 2024/9/3.
//
#include "ECS/System/AdPhongMaterialSystem.h"

#include "AdFileUtil.h"
#include "AdApplication.h"
#include "Graphic/AdVKPipeline.h"
#include "Graphic/AdVKDescriptorSet.h"
#include "Graphic/AdVKImageView.h"
#include "Graphic/AdVKFrameBuffer.h"

#include "Render/AdRenderTarget.h"

#include "ECS/Component/AdTransformComponent.h"
namespace ade{

void AdPhongMaterialSystem::OnInit(AdVKRenderPass *renderPass) {
        AdVKDevice *device = GetDevice();

        //Frame Ubo
        {
             const std::vector<VkDescriptorSetLayoutBinding> bindings = {
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                }
            };
            mFrameUboDescSetLayout = std::make_shared<AdVKDescriptorSetLayout>(device, bindings);
        }

        // Material Params
        {
            const std::vector<VkDescriptorSetLayoutBinding> bindings = {
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
                }
            };
            mMaterialParamDescSetLayout = std::make_shared<AdVKDescriptorSetLayout>(device, bindings);
        }

        // Material Resource
        {
            const std::vector<VkDescriptorSetLayoutBinding> bindings = {
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
                }
            };
            mMaterialResourceDescSetLayout = std::make_shared<AdVKDescriptorSetLayout>(device, bindings);
        }
        // Light UBO
        {
                const std::vector<VkDescriptorSetLayoutBinding> bindings = {
                    {
                        .binding = 0,
                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .descriptorCount = 1,
                        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    }
                };
                mLightDescSetLayout = std::make_shared<AdVKDescriptorSetLayout>(device, bindings);
        }
        VkPushConstantRange modelPC = {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(ModelPC)
        };

        ShaderLayout shaderLayout = {
            .descriptorSetLayouts = { mFrameUboDescSetLayout->GetHandle(), mMaterialParamDescSetLayout->GetHandle(),
                mMaterialResourceDescSetLayout->GetHandle() ,mLightDescSetLayout->GetHandle()},
            .pushConstants = { modelPC }
        };
        mPipelineLayout = std::make_shared<AdVKPipelineLayout>(device,
                                                               AD_RES_SHADER_DIR"04_phong_material.vert",
                                                               AD_RES_SHADER_DIR"04_phong_material.frag",
                                                               shaderLayout);

        std::vector<VkVertexInputBindingDescription> vertexBindings = {
            {
                .binding = 0,
                .stride = sizeof(AdVertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            }
        };
        std::vector<VkVertexInputAttributeDescription> vertexAttrs = {
            {
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(AdVertex, position)
            },
            {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(AdVertex, texcoord0)
            },
            {
                .location = 2,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(AdVertex, normal)
            },
            {
                .location = 3,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(AdVertex, color)
            }
        };
        mPipeline = std::make_shared<AdVKPipeline>(device, renderPass, mPipelineLayout.get());
        mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
        mPipeline->EnableDepthTest();
        mPipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
        mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_4_BIT, VK_FALSE);
        mPipeline->Create();

        std::vector<VkDescriptorPoolSize> poolSizes = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1
            }
        };
        mDescriptorPool = std::make_shared<AdVKDescriptorPool>(device, 10, poolSizes);
        mFrameUboDescSet = mDescriptorPool->AllocateDescriptorSet(mFrameUboDescSetLayout.get(), 1)[0];
        mLightUboDescSet = mDescriptorPool->AllocateDescriptorSet(mLightDescSetLayout.get(), 1)[0];
        mFrameUboBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(FrameUbo), nullptr, true);
        mLightUboBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(LightUbo), nullptr, true);
        ReCreateMaterialDescPool(NUM_MATERIAL_BATCH);
    }

    void AdPhongMaterialSystem::OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) {
        AdScene *scene = GetScene();
        if(!scene){
            return;
        }
        entt::registry &reg = scene->GetEcsRegistry();

        auto view = reg.view<AdTransformComponent, AdPhongMaterialComponent>();
        if(view.begin() == view.end()){
            return;
        }

        mPipeline->Bind(cmdBuffer);
        AdVKFrameBuffer *frameBuffer = renderTarget->GetFrameBuffer();
        VkViewport viewport = {
            .x = 0,
            .y = 0,
            .width = static_cast<float>(frameBuffer->GetWidth()),
            .height = static_cast<float>(frameBuffer->GetHeight()),
            .minDepth = 0.f,
            .maxDepth = 1.f
        };
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
        VkRect2D scissor = {
            .offset = { 0, 0 },
            .extent = { frameBuffer->GetWidth(), frameBuffer->GetHeight() }
        };
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

        UpdateFrameUboDescSet(renderTarget);
        UpdateLightUboDescSet();
        bool bShouldForceUpdateMaterial = false;
        uint32_t materialCount = AdMaterialFactory::GetInstance()->GetMaterialSize<AdPhongMaterial>();
        if(materialCount > mLastDescriptorSetCount){
            ReCreateMaterialDescPool(materialCount);
            bShouldForceUpdateMaterial = true;
        }

        std::vector<bool> updateFlags(materialCount);
        view.each([this, &updateFlags, &bShouldForceUpdateMaterial, &cmdBuffer](AdTransformComponent &transComp, AdPhongMaterialComponent &materialComp){
            for (const auto &entry: materialComp.GetMeshMaterials()){
                AdPhongMaterial *material = materialComp.GetMeshMaterial(entry.first);;
                if(!material || material->GetIndex() < 0){
                    LOG_W("TODO: default material or error material ?");
                    continue;
                }

                uint32_t materialIndex = material->GetIndex();
                VkDescriptorSet paramsDescSet = mMaterialDescSets[materialIndex];
                VkDescriptorSet resourceDescSet = mMaterialResourceDescSets[materialIndex];
                // todo 暂时不更新
                if(!updateFlags[materialIndex]){
                    if(material->ShouldFlushParams() || bShouldForceUpdateMaterial){
                        //LOG_T("Update material params : {0}", materialIndex);
                        UpdateMaterialParamsDescSet(paramsDescSet, material);
                        // modify light

                        material->FinishFlushParams();
                    }
                    if(material->ShouldFlushResource() || bShouldForceUpdateMaterial){
                        //LOG_T("Update material resource : {0}", materialIndex);
                        UpdateMaterialResourceDescSet(resourceDescSet, material);
                        material->FinishFlushResource();
                    }
                    updateFlags[materialIndex] = true;
                }

                VkDescriptorSet descriptorSets[] = { mFrameUboDescSet, paramsDescSet, resourceDescSet, mLightUboDescSet};
                vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(),
                                        0, ARRAY_SIZE(descriptorSets), descriptorSets, 0, nullptr);

                ModelPC pc = { transComp.GetTransform() ,};
                vkCmdPushConstants(cmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), &pc);

                for (const auto &meshIndex: entry.second){
                    materialComp.GetMesh(meshIndex)->Draw(cmdBuffer);
                }
            }
        });
    }

    void AdPhongMaterialSystem::OnDestroy() {

    }

    void AdPhongMaterialSystem::ReCreateMaterialDescPool(uint32_t materialCount) {
        AdVKDevice *device = GetDevice();

        uint32_t newDescriptorSetCount = mLastDescriptorSetCount;
        if(mLastDescriptorSetCount == 0){
            newDescriptorSetCount = NUM_MATERIAL_BATCH;
        }

        while (newDescriptorSetCount < materialCount) {
            newDescriptorSetCount *= 2;
        }

        if(newDescriptorSetCount > NUM_MATERIAL_BATCH_MAX){
            LOG_E("Descriptor Set max count is : {0}, but request : {1}", NUM_MATERIAL_BATCH_MAX, newDescriptorSetCount);
            return;
        }

        LOG_W("{0}: {1} -> {2} S.", __FUNCTION__, mLastDescriptorSetCount, newDescriptorSetCount);

        // Destroy old
        mMaterialDescSets.clear();
        mMaterialResourceDescSets.clear();
        if(mMaterialDescriptorPool){
            mMaterialDescriptorPool.reset();
        }

        std::vector<VkDescriptorPoolSize> poolSizes = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = newDescriptorSetCount
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = newDescriptorSetCount               // because has color_tex0 and color_tex1
            }
        };
        mMaterialDescriptorPool = std::make_shared<ade::AdVKDescriptorPool>(device, newDescriptorSetCount * 2, poolSizes);  //because has params and resource desc. set

        mMaterialDescSets = mMaterialDescriptorPool->AllocateDescriptorSet(mMaterialParamDescSetLayout.get(), newDescriptorSetCount);
        mMaterialResourceDescSets = mMaterialDescriptorPool->AllocateDescriptorSet(mMaterialResourceDescSetLayout.get(), newDescriptorSetCount);
        assert(mMaterialDescSets.size() == newDescriptorSetCount && "Failed to AllocateDescriptorSet");
        assert(mMaterialResourceDescSets.size() == newDescriptorSetCount && "Failed to AllocateDescriptorSet");

        uint32_t diffCount = newDescriptorSetCount - mLastDescriptorSetCount;
        for(int i = 0; i < diffCount; i++){
            mMaterialBuffers.push_back(std::make_shared<AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(PhongMaterialUbo), nullptr, true));
        }
        LOG_W("{0}: {1} -> {2} E.", __FUNCTION__, mLastDescriptorSetCount, newDescriptorSetCount);
        mLastDescriptorSetCount = newDescriptorSetCount;
    }

    void AdPhongMaterialSystem::UpdateFrameUboDescSet(AdRenderTarget *renderTarget) {
        AdApplication *app = GetApp();
        AdVKDevice *device = GetDevice();

        AdVKFrameBuffer *frameBuffer = renderTarget->GetFrameBuffer();
        glm::ivec2 resolution = { frameBuffer->GetWidth(), frameBuffer->GetHeight() };

        FrameUbo frameUbo = {
            .projMat = GetProjMat(renderTarget),
            .viewMat = GetViewMat(renderTarget),
            .resolution = resolution,
            .frameId = static_cast<uint32_t>(app->GetFrameIndex()),
            .time = app->GetStartTimeSecond()
        };

        mFrameUboBuffer->WriteData(&frameUbo);
        VkDescriptorBufferInfo bufferInfo = DescriptorSetWriter::BuildBufferInfo(mFrameUboBuffer->GetHandle(), 0, sizeof(frameUbo));
        VkWriteDescriptorSet bufferWrite = DescriptorSetWriter::WriteBuffer(mFrameUboDescSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo);
        DescriptorSetWriter::UpdateDescriptorSets(device->GetHandle(), { bufferWrite });
    }

    void AdPhongMaterialSystem::UpdateLightUboDescSet() {
        AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        AdVKDevice *device = renderCxt->GetDevice();

        AdScene *scene = GetScene();
        entt::registry &reg = scene->GetEcsRegistry();


        uint32_t pointLightCount = 0;
        reg.view<AdTransformComponent, AdPointLightComponent>()
                .each([&pointLightCount, this](AdTransformComponent &transComp, AdPointLightComponent &lightComp){
            if(pointLightCount >= LIGHT_MAX_COUNT){
                return;
            }
            lightComp.params.position = transComp.GetPosition();
            mLightUbo.pointLights[pointLightCount++] = lightComp.params;
        });


        mLightUboBuffer->WriteData(&mLightUbo);
        VkDescriptorBufferInfo bufferInfo = DescriptorSetWriter::BuildBufferInfo(mLightUboBuffer->GetHandle(), 0, sizeof(mLightUbo));
        VkWriteDescriptorSet bufferWrite = DescriptorSetWriter::WriteBuffer(mLightUboDescSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo);
        DescriptorSetWriter::UpdateDescriptorSets(device->GetHandle(), { bufferWrite });
    }

    void AdPhongMaterialSystem::UpdateMaterialParamsDescSet(VkDescriptorSet descSet, AdPhongMaterial *material) {
        AdVKDevice *device = GetDevice();

        AdVKBuffer *materialBuffer = mMaterialBuffers[material->GetIndex()].get();

        PhongMaterialUbo params = material->GetParams();

        const TextureView *texture0 = material->GetTextureView(0);
        if(texture0){
            AdMaterial::UpdateTextureParams(texture0, &params.textureParam0);
        }


        materialBuffer->WriteData(&params);
        VkDescriptorBufferInfo bufferInfo = DescriptorSetWriter::BuildBufferInfo(materialBuffer->GetHandle(), 0, sizeof(params));
        VkWriteDescriptorSet bufferWrite = DescriptorSetWriter::WriteBuffer(descSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo);
        DescriptorSetWriter::UpdateDescriptorSets(device->GetHandle(), { bufferWrite });
    }

    void AdPhongMaterialSystem::UpdateMaterialResourceDescSet(VkDescriptorSet descSet, AdPhongMaterial *material) {
        AdVKDevice *device = GetDevice();

        const TextureView *texture0 = material->GetTextureView(0);

        VkDescriptorImageInfo textureInfo0 = DescriptorSetWriter::BuildImageInfo(texture0->sampler->GetHandle(), texture0->texture->GetImageView()->GetHandle());

        VkWriteDescriptorSet textureWrite0 = DescriptorSetWriter::WriteImage(descSet, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &textureInfo0);

        DescriptorSetWriter::UpdateDescriptorSets(device->GetHandle(), { textureWrite0 });
    }

}