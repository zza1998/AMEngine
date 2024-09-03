#ifndef ADVKSWAPCHAIN_H
#define ADVKSWAPCHAIN_H

#include "AdVKCommon.h"

namespace ade{
    class AdVKGraphicContext;
    class AdVKDevice;

    struct SurfaceInfo{
        VkSurfaceCapabilitiesKHR capabilities;
        VkSurfaceFormatKHR surfaceFormat;
        VkPresentModeKHR presentMode;
    };

    class AdVKSwapchain{
    public:
        AdVKSwapchain(AdVKGraphicContext *context, AdVKDevice *device);
        ~AdVKSwapchain();

        bool ReCreate();

        VkResult AcquireImage(int32_t *outImageIndex, VkSemaphore semaphore, VkFence fence = VK_NULL_HANDLE);
        VkResult Present(int32_t imageIndex, const std::vector<VkSemaphore> &waitSemaphores);

        const std::vector<VkImage> &GetImages() const { return mImages; }
        uint32_t GetWidth() const { return mSurfaceInfo.capabilities.currentExtent.width; }
        uint32_t GetHeight() const { return mSurfaceInfo.capabilities.currentExtent.height; }
        int32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }

        const SurfaceInfo &GetSurfaceInfo() const { return mSurfaceInfo; }
    private:
        void SetupSurfaceCapabilities();

        VkSwapchainKHR mHandle = VK_NULL_HANDLE;

        AdVKGraphicContext *mContext;
        AdVKDevice *mDevice;
        std::vector<VkImage> mImages;

        int32_t mCurrentImageIndex = -1;

        SurfaceInfo mSurfaceInfo;
    };
}

#endif