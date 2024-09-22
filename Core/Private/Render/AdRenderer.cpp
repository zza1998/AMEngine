#include "Render/AdRenderer.h"
#include "AdApplication.h"
#include "Graphic/AdVKQueue.h"

namespace ade{
    AdRenderer::AdRenderer() {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();

        mImageAvailableSemaphores.resize(RENDERER_NUM_BUFFER);
        mSubmitedSemaphores.resize(RENDERER_NUM_BUFFER);
        mRenderFinishedSemaphores.resize(RENDERER_NUM_BUFFER);
        mFrameFences.resize(RENDERER_NUM_BUFFER);
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
        for(int i = 0; i < RENDERER_NUM_BUFFER; i++){
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]));
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mSubmitedSemaphores[i]));
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]));
            CALL_VK(vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &mFrameFences[i]));
        }
    }

    AdRenderer::~AdRenderer() {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        for (const auto &item: mImageAvailableSemaphores){
            VK_D(Semaphore, device->GetHandle(), item);
        }
        for (const auto &item: mSubmitedSemaphores){
            VK_D(Semaphore, device->GetHandle(), item);
        }
        for (const auto &item: mRenderFinishedSemaphores){
            VK_D(Semaphore, device->GetHandle(), item);
        }
        for (const auto &item: mFrameFences){
            VK_D(Fence, device->GetHandle(), item);
        }
    }

    bool AdRenderer::Begin(int32_t *outImageIndex) {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        bool bShouldUpdateTarget = false;

        CALL_VK(vkWaitForFences(device->GetHandle(), 1, &mFrameFences[mCurrentBuffer], VK_TRUE, UINT64_MAX));
        CALL_VK(vkResetFences(device->GetHandle(), 1, &mFrameFences[mCurrentBuffer]));

        VkResult ret = swapchain->AcquireImage(outImageIndex, mImageAvailableSemaphores[mCurrentBuffer]);
        if(ret == VK_ERROR_OUT_OF_DATE_KHR){
            CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
            VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            bool bSuc = swapchain->ReCreate();

            VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
                bShouldUpdateTarget = true;
            }
            ret = swapchain->AcquireImage(outImageIndex, mImageAvailableSemaphores[mCurrentBuffer]);
            if(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR){
                LOG_E("Recreate swapchain error: {0}", vk_result_string(ret));
            }
        }
        return bShouldUpdateTarget;
    }

    bool AdRenderer::End(int32_t imageIndex, const std::vector<VkCommandBuffer> &cmdBuffers) {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();
        bool bShouldUpdateTarget = false;

        device->GetFirstGraphicQueue()->Submit(cmdBuffers, { mImageAvailableSemaphores[mCurrentBuffer] }, { mSubmitedSemaphores[mCurrentBuffer] }, mFrameFences[mCurrentBuffer]);

        VkResult ret = swapchain->Present(imageIndex, { mSubmitedSemaphores[mCurrentBuffer] });
        if(ret == VK_SUBOPTIMAL_KHR || ret == VK_ERROR_OUT_OF_DATE_KHR){
            CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
            VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            bool bSuc = swapchain->ReCreate();

            VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
                bShouldUpdateTarget = true;
            }
        }
        else if (ret != VK_SUCCESS) {
            LOG_E("{0} present error :{1} ,check queue info pls", __FILENAME__, ret);
        }
        // if we do not add vkDeviceWaitIdle, we will get a vulkan error:
        // Cannot call vkUpdateDescriptorSets() to perform write update on VkDescriptorSet xxx allocated with VkDescriptorSetLayout xxx that is in use by a command buffer.
        CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
        mCurrentBuffer = (mCurrentBuffer + 1) % RENDERER_NUM_BUFFER;
        return bShouldUpdateTarget;
    }

    void AdRenderer::Present() {

    }
}
