//
// Created by zhouzian on 2024/9/16.
//
#include "ECS\System\AdSkyBoxSystem.h"
#include "Graphic/AdVKPipeline.h"
#include "Graphic/AdVKDescriptorSet.h"
#include "Graphic/AdVKImageView.h"
#include "Graphic/AdVKFrameBuffer.h"

namespace ade {
    /*渲染顺序：先渲染天空盒，然后再渲染普通物体。
     深度设置：渲染天空盒时禁用深度写入，但保留深度测试。渲染普通物体时启用深度写入。
     移除视图矩阵的平移部分：天空盒的视图矩阵应去掉摄像机的位置偏移，只保留旋转。
     清除缓冲区：在开始渲染每一帧之前，清除颜色和深度缓冲区。*/
    void AdSkyBoxSystem::OnInit(AdVKRenderPass *renderPass) {
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
        VkPushConstantRange modelPC = {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(ModelPC)
        };

        ShaderLayout shaderLayout = {
            .descriptorSetLayouts = {
                mFrameUboDescSetLayout->GetHandle(), mMaterialResourceDescSetLayout->GetHandle(),
            },
            .pushConstants = {modelPC}
        };
        mPipelineLayout = std::make_shared<AdVKPipelineLayout>(device,
                                                               AD_RES_SHADER_DIR"skybox.vert",
                                                               AD_RES_SHADER_DIR"skybox.frag",
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
            }
        };
        mPipeline = std::make_shared<AdVKPipeline>(device, renderPass, mPipelineLayout.get());
        mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
        mPipeline->DisableDepthWriteButTest(); // need disable for skybox
        mPipeline->SetCullingMode(VK_CULL_MODE_FRONT_BIT); // cull front
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
        mFrameUboDescSet = mDescriptorPool->AllocateDescriptorSet(mFrameUboDescSetLayout.get(), 1)[0];
        mFrameUboBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                            sizeof(FrameUbo), nullptr, true);
        ReCreateMaterialDescPool(NUM_MATERIAL_BATCH);
    }

    void AdSkyBoxSystem::OnDestroy() {

    }

    void AdSkyBoxSystem::OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) {
        AdScene* scene = GetScene();
        if (!scene) {
            return;
        }
        entt::registry& reg = scene->GetEcsRegistry();

        auto view = reg.view<AdTransformComponent, AdPhongMaterialComponent>();
        if (view.begin() == view.end()) {
            return;
        }

        mPipeline->Bind(cmdBuffer);
        AdVKFrameBuffer* frameBuffer = renderTarget->GetFrameBuffer();
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
        bool bShouldForceUpdateMaterial = false;
        uint32_t materialCount = AdMaterialFactory::GetInstance()->GetMaterialSize<AdPhongMaterial>();
        if (materialCount > mLastDescriptorSetCount) {
            ReCreateMaterialDescPool(materialCount);
            bShouldForceUpdateMaterial = true;
        }

        std::vector<bool> updateFlags(materialCount);
        view.each([this, &updateFlags, &bShouldForceUpdateMaterial, &cmdBuffer](AdTransformComponent& transComp, AdPhongMaterialComponent& materialComp) {
            for (const auto& entry : materialComp.GetMeshMaterials()) {
                AdPhongMaterial* material = entry.first;
                if (!material || material->GetIndex() < 0) {
                    LOG_W("TODO: default material or error material ?");
                    continue;
                }

                uint32_t materialIndex = material->GetIndex();
                VkDescriptorSet resourceDescSet = mMaterialResourceDescSets[materialIndex];
                // todo 暂时不更新
                if (!updateFlags[materialIndex]) {
                    if (material->ShouldFlushResource() || bShouldForceUpdateMaterial) {
                        //LOG_T("Update material resource : {0}", materialIndex);
                        UpdateMaterialResourceDescSet(resourceDescSet, material);
                        material->FinishFlushResource();
                    }
                    updateFlags[materialIndex] = true;
                }

                VkDescriptorSet descriptorSets[] = { mFrameUboDescSet, resourceDescSet };
                vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(),
                    0, ARRAY_SIZE(descriptorSets), descriptorSets, 0, nullptr);

                ModelPC pc = { transComp.GetTransform() , };
                vkCmdPushConstants(cmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), &pc);

                for (const auto& meshIndex : entry.second) {
                    materialComp.GetMesh(meshIndex)->Draw(cmdBuffer);
                }
            }
            });

    }
}
