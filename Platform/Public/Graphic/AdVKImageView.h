#ifndef ADVKIMAGEVIEW_H
#define ADVKIMAGEVIEW_H

#include "Graphic/AdVKCommon.h"

namespace ade{
    class AdVKDevice;

    class AdVKImageView{
    public:
        AdVKImageView(AdVKDevice *device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        ~AdVKImageView();

        VkImageView GetHandle() const { return mHandle; }
        VkImageView * GetHandlePtr() { return &mHandle; }
    private:
        VkImageView mHandle = VK_NULL_HANDLE;

        AdVKDevice *mDevice;
    };
}

#endif