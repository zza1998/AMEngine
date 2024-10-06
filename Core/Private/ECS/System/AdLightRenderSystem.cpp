//
// Created by zhouzian on 2024/10/4.
//

#include "ECS/System/AdLightRenderSystem.h"

#include <ECS/Component/AdTransformComponent.h>

#include "AdFileUtil.h"
#include "Graphic/AdVKPipeline.h"
#include "Graphic/AdVKDescriptorSet.h"
#include "Render/AdRenderTarget.h"
namespace ade {
    void AdLightRenderSystem::OnInit(AdVKRenderPass *renderPass) {
        AdVKDevice *device = GetDevice();
        // Material Resource
        {
            const std::vector<VkDescriptorSetLayoutBinding> bindings = {
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
                },{
                    .binding = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
                },{
                    .binding = 2,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
                },{
                    .binding = 3,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
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
        // Frame UBO
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
        ShaderLayout shaderLayout = {
            .descriptorSetLayouts = {
                mMaterialResourceDescSetLayout->GetHandle(), mLightDescSetLayout->GetHandle(),mFrameUboDescSetLayout->GetHandle()
            }
        };
        mPipelineLayout = std::make_shared<AdVKPipelineLayout>(device,
                                                               AD_RES_SHADER_DIR"light.vert",
                                                               AD_RES_SHADER_DIR"light.frag",
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
        mPipeline->SetSubPassNo(1);
        mPipeline->DisableDepthTestAndWrite();
        mPipeline->SetDynamicState({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
        mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_4_BIT, VK_FALSE);
        mPipeline->Create();
        std::vector<VkDescriptorPoolSize> poolSizes = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1
            }
        };
        mDescriptorPool = std::make_shared<AdVKDescriptorPool>(device, 10, poolSizes);
        mLightUboDescSet = mDescriptorPool->AllocateDescriptorSet(mLightDescSetLayout.get(), 1)[0];
        mFrameUboDescSet = mDescriptorPool->AllocateDescriptorSet(mFrameUboDescSetLayout.get(), 1)[0];
        mGbufferUboDescSet = mDescriptorPool->AllocateDescriptorSet(mMaterialResourceDescSetLayout.get(), 1)[0];
        mLightUboBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                            sizeof(LightUbo), nullptr, true);
        mFrameUboBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(FrameUbo), nullptr, true);

    }

    void AdLightRenderSystem::OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) {
        AdScene *scene = GetScene();
        if(!scene){
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

        UpdateLightUboDescSet();
        UpdateFrameUboDescSet(renderTarget);
        UpdateGbufferUboDescSet(renderTarget);
        // vkUpdateDescriptorSet
        VkDescriptorSet descriptorSets[] = {mGbufferUboDescSet, mLightUboDescSet, mFrameUboDescSet};
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,mPipelineLayout->GetHandle(),
            0, ARRAY_SIZE(descriptorSets), descriptorSets, 0, nullptr);
        // vkBindDescriptorSets
        vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
    }

    void AdLightRenderSystem::OnDestroy() {

    }

    void AdLightRenderSystem::UpdateGbufferUboDescSet(AdRenderTarget *renderTarget) {
        AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        AdVKDevice *device = renderCxt->GetDevice();
        AdVKFrameBuffer *frameBuffer = renderTarget->GetFrameBuffer();
        auto imageViews = frameBuffer->GetImageViews();
        // Input attachment更新
        const int32_t len = 4;
        VkDescriptorImageInfo imageInfo[len] = {
            {
                .imageView = imageViews[0]->GetHandle(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            },
            {
                .imageView = imageViews[1]->GetHandle(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            },
            {
                .imageView = imageViews[2]->GetHandle(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            },
            {
                .imageView = imageViews[3]->GetHandle(),
                .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            }
        };

        VkWriteDescriptorSet writeSets[len];
        // 更新binding 0
        for(int i = 0;i < len;i++) {
            writeSets[i] = {
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = mGbufferUboDescSet,
                    .dstBinding = static_cast<uint32_t>(i),
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .pImageInfo = &imageInfo[i],
            };

        }

        // 更新描述符集
        vkUpdateDescriptorSets(device->GetHandle(), ARRAY_SIZE(writeSets), writeSets, 0, nullptr);
    }

    void AdLightRenderSystem::UpdateLightUboDescSet() {
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

    void AdLightRenderSystem::UpdateFrameUboDescSet(AdRenderTarget *renderTarget) {
        AdApplication *app = GetApp();
        AdVKDevice *device = GetDevice();

        AdVKFrameBuffer *frameBuffer = renderTarget->GetFrameBuffer();
        glm::ivec2 resolution = { frameBuffer->GetWidth(), frameBuffer->GetHeight() };

        FrameUbo frameUbo = {
            .projMat = GetProjMat(renderTarget),
            .viewMat = GetViewMat(renderTarget),
            .resolution = resolution,
            .frameId = static_cast<uint32_t>(app->GetFrameIndex()),
            .time = app->GetStartTimeSecond(),
            .cameraPos = GetCameraPos(renderTarget)
        };

        mFrameUboBuffer->WriteData(&frameUbo);
        VkDescriptorBufferInfo bufferInfo = DescriptorSetWriter::BuildBufferInfo(mFrameUboBuffer->GetHandle(), 0, sizeof(frameUbo));
        VkWriteDescriptorSet bufferWrite = DescriptorSetWriter::WriteBuffer(mFrameUboDescSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo);
        DescriptorSetWriter::UpdateDescriptorSets(device->GetHandle(), { bufferWrite });
    }

}
