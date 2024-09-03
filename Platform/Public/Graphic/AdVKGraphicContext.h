#ifndef AD_VK_GRAPHIC_CONTEXT_H
#define AD_VK_GRAPHIC_CONTEXT_H

#include "AdGraphicContext.h"
#include "AdVKCommon.h"

namespace ade{
    struct QueueFamilyInfo{
        int32_t queueFamilyIndex = -1;
        uint32_t queueCount;
    };

    class AdVKGraphicContext : public AdGraphicContext{
    public:
        AdVKGraphicContext(AdWindow *window);
        ~AdVKGraphicContext() override;

        VkInstance GetInstance() const { return mInstance; }
        VkSurfaceKHR GetSurface() const { return mSurface; }
        VkPhysicalDevice GetPhyDevice() const { return mPhyDevice; }
        const QueueFamilyInfo &GetGraphicQueueFamilyInfo() const { return mGraphicQueueFamily; }
        const QueueFamilyInfo &GetPresentQueueFamilyInfo() const { return mPresentQueueFamily; }
        VkPhysicalDeviceMemoryProperties GetPhyDeviceMemProperties() const { return mPhyDeviceMemProperties; }
        bool IsSameGraphicPresentQueueFamily() const { return mGraphicQueueFamily.queueFamilyIndex == mPresentQueueFamily.queueFamilyIndex; }
    private:
        static void PrintPhyDeviceInfo(VkPhysicalDeviceProperties &props);
        static uint32_t GetPhyDeviceScore(VkPhysicalDeviceProperties &props);

        void CreateInstance();
        void CreateSurface(AdWindow *window);
        void SelectPhyDevice();

        bool bShouldValidate = true;
        VkInstance mInstance;
        VkSurfaceKHR mSurface;

        VkPhysicalDevice mPhyDevice;
        QueueFamilyInfo mGraphicQueueFamily;
        QueueFamilyInfo mPresentQueueFamily;
        VkPhysicalDeviceMemoryProperties mPhyDeviceMemProperties;
    };
}

#endif