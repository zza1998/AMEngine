#ifndef ADVKIMAGE_H
#define ADVKIMAGE_H

#include "AdVKCommon.h"

namespace ade{
    class AdVKDevice;
    class AdVKBuffer;

    class AdVKImage{
    public:
        AdVKImage(AdVKDevice *device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
        AdVKImage(AdVKDevice *device, VkImage image, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
        AdVKImage(AdVKDevice *device,VkExtent3D extent,VkFormat format, int useCubeSample);
        ~AdVKImage();

        static bool TransitionLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,uint16_t imageNum = 1);

        void CopyFromBuffer(VkCommandBuffer cmdBuffer, AdVKBuffer *buffer);

        VkFormat GetFormat() const { return mFormat; }
        VkImage GetHandle() const { return mHandle; }
    private:
        VkImage mHandle = VK_NULL_HANDLE;
        VkDeviceMemory mMemory  =VK_NULL_HANDLE;

        bool bCreateImage = true;

        AdVKDevice *mDevice;

        VkFormat mFormat;
        VkExtent3D mExtent;
        VkImageUsageFlags mUsage;
    };
}

#endif