#ifndef AD_VKBUFFER_H
#define AD_VKBUFFER_H

#include "AdVKCommon.h"

namespace ade {
    class AdVKDevice;

    class AdVKBuffer{
    public:
        AdVKBuffer(AdVKDevice *device, VkBufferUsageFlags usage, size_t size, void *data = nullptr, bool bHostVisible = false);
        ~AdVKBuffer();

        static void CreateBufferInternal(AdVKDevice *device, VkMemoryPropertyFlags memProps, VkBufferUsageFlags usage, size_t size, VkBuffer *outBuffer, VkDeviceMemory *outMemory);
        static void CopyToBuffer(AdVKDevice *device, VkBuffer srcBuffer, VkBuffer dstBuffer, size_t size);

        VkResult WriteData(void *data);

        VkBuffer GetHandle() const { return mHandle; }
    private:
        void CreateBuffer(VkBufferUsageFlags usage, void *data);

        VkBuffer mHandle = VK_NULL_HANDLE;
        VkDeviceMemory mMemory = VK_NULL_HANDLE;

        AdVKDevice *mDevice;
        size_t mSize;
        bool bHostVisible;
    };
}
#endif