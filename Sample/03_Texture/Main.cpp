#include "AdEntryPoint.h"
#include "AdFileUtil.h"
#include "Render/AdRenderTarget.h"
#include "Render/AdMesh.h"
#include "Render/AdTexture.h"
#include "Render/AdSampler.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKPipeline.h"
#include "Graphic/AdVKCommandBuffer.h"
#include "Graphic/AdVKDescriptorSet.h"
#include "Graphic/AdVKImageView.h"

struct GlobalUbo{
    glm::mat4 projMat{ 1.f };
    glm::mat4 viewMat{ 1.f };
};

struct InstanceUbo{
    glm::mat4 modelMat{ 1.f };
};

class SandBoxApp : public ade::AdApplication{
protected:
    void OnConfiguration(ade::AppSettings *appSettings) override {
        appSettings->width = 1360;
        appSettings->height = 768;
        appSettings->title = "03_Texture";
    }

    void OnInit() override {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        std::vector<ade::Attachment> attachments = {
            {
                .format = swapchain->GetSurfaceInfo().surfaceFormat.format,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
            },
            {
                .format = device->GetSettings().depthFormat,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
            }
        };
        std::vector<ade::RenderSubPass> subpasses = {
            {
                .colorAttachments = { 0 },
                .depthStencilAttachments = { 1 },
                .sampleCount = VK_SAMPLE_COUNT_4_BIT
            }
        };
        mRenderPass = std::make_shared<ade::AdVKRenderPass>(device, attachments, subpasses);

        mRenderTarget = std::make_shared<ade::AdRenderTarget>(mRenderPass.get());
        mRenderTarget->SetColorClearValue({0.1f, 0.2f, 0.3f, 1.f});
        mRenderTarget->SetDepthStencilClearValue({ 1, 0 });

        std::vector<VkDescriptorSetLayoutBinding> desctLayoutBindings = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
            },
            {
                .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
            },
            {
                .binding = 3,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
            }
        };
        mDescriptorSetLayout = std::make_shared<ade::AdVKDescriptorSetLayout>(device, desctLayoutBindings);

        ade::ShaderLayout shaderLayout = {
                .descriptorSetLayouts = { mDescriptorSetLayout->GetHandle() }
        };
        mPipelineLayout = std::make_shared<ade::AdVKPipelineLayout>(device,
                                                                    AD_RES_SHADER_DIR"02_descriptor_set.vert",
                                                                    AD_RES_SHADER_DIR"02_descriptor_set.frag",
                                                                    shaderLayout);
        std::vector<VkVertexInputBindingDescription> vertexBindings = {
            {
                .binding = 0,
                .stride = sizeof(ade::AdVertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            }
        };
        std::vector<VkVertexInputAttributeDescription> vertexAttrs = {
            {
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(ade::AdVertex, position)
            },
            {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(ade::AdVertex, texcoord0)
            },
            {
                .location = 2,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(ade::AdVertex, normal)
            }
        };
        mPipeline = std::make_shared<ade::AdVKPipeline>(device, mRenderPass.get(), mPipelineLayout.get());
        mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
        mPipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
        mPipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
        mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_4_BIT, VK_FALSE);
        mPipeline->Create();

        std::vector<VkDescriptorPoolSize> poolSizes = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 2
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 2
            }
        };
        mDescriptorPool = std::make_shared<ade::AdVKDescriptorPool>(device, 1, poolSizes);
        mDescriptorSets = mDescriptorPool->AllocateDescriptorSet(mDescriptorSetLayout.get(), 1);

        mGlobalBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(GlobalUbo), nullptr, true);
        mInstanceBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(InstanceUbo), nullptr, true);
        mSampler = std::make_shared<ade::AdSampler>();
        mTexture0 = std::make_shared<ade::AdTexture>(AD_RES_TEXTURE_DIR"awesomeface.png");
        mTexture1 = std::make_shared<ade::AdTexture>(AD_RES_TEXTURE_DIR"R-C.jpeg");

        mImageAvailableSemaphores.resize(mNumBuffer);
        mSubmitedSemaphores.resize(mNumBuffer);
        mFrameFences.resize(mNumBuffer);
        VkSemaphoreCreateInfo semaphoreInfo = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0
        };
        VkFenceCreateInfo fenceInfo = {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT

        };
        for(int i = 0; i < mNumBuffer; i++){
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]));
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mSubmitedSemaphores[i]));
            CALL_VK(vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &mFrameFences[i]));
        }

        mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffer(swapchain->GetImages().size());
        std::vector<ade::AdVertex> vertices;
        std::vector<uint32_t> indices;
        ade::AdGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
        mCubeMesh = std::make_shared<ade::AdMesh>(vertices, indices);
    }

    void OnUpdate(float deltaTime) override {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - mStartTimePoint).count();
        mInstanceUbo.modelMat = glm::rotate(glm::mat4(1.f), glm::radians(17.f), glm::vec3(1, 0, 0));
        mInstanceUbo.modelMat = glm::rotate(mInstanceUbo.modelMat, glm::radians(time * 100.f), glm::vec3(0, 1, 0));

        mGlobalUbo.projMat = glm::perspective(glm::radians(65.f), swapchain->GetWidth() * 1.f / swapchain->GetHeight(), 0.01f, 100.f);
        mGlobalUbo.projMat[1][1] *= -1.f;
        mGlobalUbo.viewMat = glm::lookAt(glm::vec3{ 0, 0, 1.5f }, glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });
    }

    void OnRender() override {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        CALL_VK(vkWaitForFences(device->GetHandle(), 1, &mFrameFences[mCurrentBuffer], VK_TRUE, UINT64_MAX));
        CALL_VK(vkResetFences(device->GetHandle(), 1, &mFrameFences[mCurrentBuffer]));

        int32_t imageIndex;
        VkResult ret = swapchain->AcquireImage(&imageIndex, mImageAvailableSemaphores[mCurrentBuffer]);
        if(ret == VK_ERROR_OUT_OF_DATE_KHR){
            CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
            VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            bool bSuc = swapchain->ReCreate();

            VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
                mRenderTarget->SetExtent(newExtent);
            }
            ret = swapchain->AcquireImage(&imageIndex, mImageAvailableSemaphores[mCurrentBuffer]);
            if(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR){
                LOG_E("Recreate swapchain error: {0}", vk_result_string(ret));
            }
        }

        VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
        ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffer);

        mRenderTarget->Begin(cmdBuffer);
        ade::AdVKFrameBuffer *frameBuffer = mRenderTarget->GetFrameBuffer();

        mPipeline->Bind(cmdBuffer);
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

        mGlobalBuffer->WriteData(&mGlobalUbo);
        mInstanceBuffer->WriteData(&mInstanceUbo);
        UpdateDescriptorSets();

        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(),
                                0, 1, mDescriptorSets.data(), 0, nullptr);

        mCubeMesh->Draw(cmdBuffer);

        mRenderTarget->End(cmdBuffer);

        ade::AdVKCommandPool::EndCommandBuffer(cmdBuffer);
        device->GetFirstGraphicQueue()->Submit({ cmdBuffer }, { mImageAvailableSemaphores[mCurrentBuffer] }, { mSubmitedSemaphores[mCurrentBuffer] }, mFrameFences[mCurrentBuffer]);
        ret = swapchain->Present(imageIndex, { mSubmitedSemaphores[mCurrentBuffer] });
        if(ret == VK_SUBOPTIMAL_KHR){
            CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
            VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            bool bSuc = swapchain->ReCreate();

            VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
                mRenderTarget->SetExtent(newExtent);
            }
        }
        CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
        mCurrentBuffer = (mCurrentBuffer + 1) % mNumBuffer;
    }

    void OnDestroy() override {
        ade::AdRenderContext *renderCxt = ade::AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        vkDeviceWaitIdle(device->GetHandle());
        mGlobalBuffer.reset();
        mInstanceBuffer.reset();
        mSampler.reset();
        mTexture0.reset();
        mTexture1.reset();
        mCubeMesh.reset();
        mCmdBuffers.clear();
        mDescriptorPool.reset();
        mDescriptorSetLayout.reset();
        mPipeline.reset();
        mPipelineLayout.reset();
        mRenderTarget.reset();
        mRenderPass.reset();
        for(int i = 0; i < mNumBuffer; i++){
            VK_D(Semaphore, device->GetHandle(), mImageAvailableSemaphores[i]);
            VK_D(Semaphore, device->GetHandle(), mSubmitedSemaphores[i]);
            VK_D(Fence, device->GetHandle(), mFrameFences[i]);
        }
    }

    void UpdateDescriptorSets(){
        ade::AdRenderContext *renderCxt = ade::AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();

        VkDescriptorBufferInfo globalBufferInfo = {
            .buffer = mGlobalBuffer->GetHandle(),
            .offset = 0,
            .range = sizeof(mGlobalUbo)
        };

        VkDescriptorBufferInfo instanceBufferInfo = {
            .buffer = mInstanceBuffer->GetHandle(),
            .offset = 0,
            .range = sizeof(mInstanceUbo)
        };

        VkDescriptorImageInfo textureImageInfo0 = {
            .sampler = mSampler->GetHandle(),
            .imageView = mTexture0->GetImageView()->GetHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkDescriptorImageInfo textureImageInfo1 = {
            .sampler = mSampler->GetHandle(),
            .imageView = mTexture1->GetImageView()->GetHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkDescriptorSet descriptorSet = mDescriptorSets[0];
        std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &globalBufferInfo
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &instanceBufferInfo
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo0
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = 3,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo1
            },
        };
        vkUpdateDescriptorSets(device->GetHandle(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }
private:
    std::shared_ptr<ade::AdVKRenderPass> mRenderPass;
    std::shared_ptr<ade::AdRenderTarget> mRenderTarget;
    std::shared_ptr<ade::AdVKDescriptorSetLayout> mDescriptorSetLayout;
    std::shared_ptr<ade::AdVKDescriptorPool> mDescriptorPool;
    std::vector<VkDescriptorSet> mDescriptorSets;
    std::shared_ptr<ade::AdVKPipelineLayout> mPipelineLayout;
    std::shared_ptr<ade::AdVKPipeline> mPipeline;

    std::shared_ptr<ade::AdMesh> mCubeMesh;
    GlobalUbo mGlobalUbo;
    InstanceUbo mInstanceUbo;
    std::shared_ptr<ade::AdVKBuffer> mGlobalBuffer;
    std::shared_ptr<ade::AdVKBuffer> mInstanceBuffer;
    std::shared_ptr<ade::AdTexture> mTexture0;
    std::shared_ptr<ade::AdTexture> mTexture1;
    std::shared_ptr<ade::AdSampler> mSampler;

    const uint32_t mNumBuffer = 2;
    uint32_t mCurrentBuffer = 0;
    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mSubmitedSemaphores;
    std::vector<VkFence> mFrameFences;

    std::vector<VkCommandBuffer> mCmdBuffers;
};

ade::AdApplication *CreateApplicationEntryPoint(){
    return new SandBoxApp();
}