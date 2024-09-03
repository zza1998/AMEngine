#ifndef AD_VKCOMMANDBUFFER_H
#define AD_VKCOMMANDBUFFER_H

#include "AdVKCommon.h"

namespace ade{
    class AdVKDevice;

    class AdVKCommandPool{
    public:
        AdVKCommandPool(AdVKDevice *device, uint32_t queueFamilyIndex);
        ~AdVKCommandPool();

        static void BeginCommandBuffer(VkCommandBuffer cmdBuffer);
        static void EndCommandBuffer(VkCommandBuffer cmdBuffer);

        std::vector<VkCommandBuffer> AllocateCommandBuffer(uint32_t count) const;
        VkCommandBuffer AllocateOneCommandBuffer() const;
        VkCommandPool GetHandle() const { return mHandle; }
    private:
        VkCommandPool mHandle;

        AdVKDevice *mDevice;
    };
}

#endif