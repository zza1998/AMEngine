#include "Graphic/AdVkSwapchain.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKQueue.h"

namespace ade{
    AdVKSwapchain::AdVKSwapchain(AdVKGraphicContext *context, AdVKDevice *device) : mContext(context), mDevice(device) {
        ReCreate();
    }

    AdVKSwapchain::~AdVKSwapchain() {
        VK_D(SwapchainKHR, mDevice->GetHandle(), mHandle);
    }

    bool AdVKSwapchain::ReCreate() {
        LOG_D("-----------------------------");
        SetupSurfaceCapabilities();
        LOG_D("currentExtent : {0} x {1}", mSurfaceInfo.capabilities.currentExtent.width, mSurfaceInfo.capabilities.currentExtent.height);
        LOG_D("surfaceFormat : {0}", vk_format_string(mSurfaceInfo.surfaceFormat.format));
        LOG_D("presentMode   : {0}", vk_present_mode_string(mSurfaceInfo.presentMode));
        LOG_D("-----------------------------");

        uint32_t imageCount = mDevice->GetSettings().swapchainImageCount;
        if(imageCount < mSurfaceInfo.capabilities.minImageCount && mSurfaceInfo.capabilities.minImageCount > 0){
            imageCount = mSurfaceInfo.capabilities.minImageCount;
        }
        if(imageCount > mSurfaceInfo.capabilities.maxImageCount && mSurfaceInfo.capabilities.maxImageCount > 0){
            imageCount = mSurfaceInfo.capabilities.maxImageCount;
        }

        VkSharingMode imageSharingMode;
        uint32_t queueFamilyIndexCount;
        uint32_t pQueueFamilyIndices[2] = { 0, 0 };
        if(mContext->IsSameGraphicPresentQueueFamily()){
            imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            queueFamilyIndexCount = 0;
        } else {
            imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            queueFamilyIndexCount = 2;
            pQueueFamilyIndices[0] = mContext->GetGraphicQueueFamilyInfo().queueFamilyIndex;
            pQueueFamilyIndices[1] = mContext->GetPresentQueueFamilyInfo().queueFamilyIndex;
        }

        VkSwapchainKHR oldSwapchain = mHandle;

        VkSwapchainCreateInfoKHR swapchainInfo = {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .surface = mContext->GetSurface(),
                .minImageCount = imageCount,
                .imageFormat = mSurfaceInfo.surfaceFormat.format,
                .imageColorSpace = mSurfaceInfo.surfaceFormat.colorSpace,
                .imageExtent = mSurfaceInfo.capabilities.currentExtent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode = imageSharingMode,
                .queueFamilyIndexCount = queueFamilyIndexCount,
                .pQueueFamilyIndices = pQueueFamilyIndices,
                .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
                //.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = mSurfaceInfo.presentMode,
                .clipped = VK_FALSE,
                .oldSwapchain = oldSwapchain
        };
        VkResult ret = vkCreateSwapchainKHR(mDevice->GetHandle(), &swapchainInfo, nullptr, &mHandle);
        if(ret != VK_SUCCESS){
            LOG_E("{0} : {1}", __FUNCTION__, vk_result_string(ret));
            return false;
        }
        LOG_T("Swapchain {0} : old: {1}, new: {2}, image count: {3}, format: {4}, present mode : {5}", __FUNCTION__, (void*)oldSwapchain, (void*)mHandle, imageCount,
              vk_format_string(mSurfaceInfo.surfaceFormat.format), vk_present_mode_string(mSurfaceInfo.presentMode));

        uint32_t swapchainImageCount;
        ret = vkGetSwapchainImagesKHR(mDevice->GetHandle(), mHandle, &swapchainImageCount, nullptr);
        mImages.resize(swapchainImageCount);
        ret = vkGetSwapchainImagesKHR(mDevice->GetHandle(), mHandle, &swapchainImageCount, mImages.data());
        return ret == VK_SUCCESS;
    }

    void AdVKSwapchain::SetupSurfaceCapabilities() {
        // capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &mSurfaceInfo.capabilities);

        AdVkSettings settings = mDevice->GetSettings();

        // format
        uint32_t formatCount;
        CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &formatCount, nullptr));
        if(formatCount == 0){
            LOG_E("{0} : num of surface format is 0", __FUNCTION__);
            return;
        }
        VkSurfaceFormatKHR* formats =new VkSurfaceFormatKHR[formatCount];
        CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &formatCount, formats));
        int32_t foundFormatIndex = -1;
        for(int i = 0; i < formatCount; i++){
            if(formats[i].format == settings.surfaceFormat && formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){
                foundFormatIndex = i;
                break;
            }
        }
        if(foundFormatIndex == -1){
            foundFormatIndex = 0;
        }
        mSurfaceInfo.surfaceFormat = formats[foundFormatIndex];

        // present mode
        uint32_t presentModeCount;
        CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &presentModeCount, nullptr));
        if(presentModeCount == 0){
            LOG_E("{0} : num of surface present mode is 0", __FUNCTION__);
            return;
        }
        VkPresentModeKHR* presentModes = new VkPresentModeKHR[presentModeCount];
        CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->GetPhyDevice(), mContext->GetSurface(), &presentModeCount, presentModes));
        VkPresentModeKHR preferredPresentMode = mDevice->GetSettings().presentMode;
        int32_t foundPresentModeIndex = -1;
        for(int i = 0; i < presentModeCount; i++){
            if(presentModes[i] == preferredPresentMode){
                foundPresentModeIndex = i;
                break;
            }
        }
        if(foundPresentModeIndex >= 0){
            mSurfaceInfo.presentMode = presentModes[foundPresentModeIndex];
        } else {
            mSurfaceInfo.presentMode = presentModes[0];
        }
    }

    VkResult AdVKSwapchain::AcquireImage(int32_t *outImageIndex, VkSemaphore semaphore, VkFence fence) {
        uint32_t imageIndex;
        VkResult ret = vkAcquireNextImageKHR(mDevice->GetHandle(), mHandle, UINT64_MAX, semaphore, fence, &imageIndex);
        if(fence != VK_NULL_HANDLE){
            CALL_VK(vkWaitForFences(mDevice->GetHandle(), 1, &fence, VK_FALSE, UINT64_MAX));
            CALL_VK(vkResetFences(mDevice->GetHandle(), 1, &fence));
        }

        if(ret == VK_SUCCESS || ret == VK_SUBOPTIMAL_KHR){
            *outImageIndex = imageIndex;
            mCurrentImageIndex = imageIndex;
        }
        else {
            LOG_E("{0} : arquireImage fail result : {1}", __FUNCTION__, ret);
        }
        return ret;
    }

    VkResult AdVKSwapchain::Present(int32_t imageIndex, const std::vector<VkSemaphore> &waitSemaphores) {
        VkPresentInfoKHR presentInfo = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = nullptr,
                .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
                .pWaitSemaphores = waitSemaphores.data(),
                .swapchainCount = 1,
                .pSwapchains = &mHandle,
                .pImageIndices = reinterpret_cast<const uint32_t *>(&imageIndex)
        };
        VkResult ret = vkQueuePresentKHR(mDevice->GetFirstPresentQueue()->GetHandle(), &presentInfo);
        mDevice->GetFirstPresentQueue()->WaitIdle();
        return ret;
    }
}