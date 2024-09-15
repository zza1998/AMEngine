#ifndef ADVKFRAMEBUFFER_H
#define ADVKFRAMEBUFFER_H

#include "AdVKImageView.h"
#include "Graphic/AdVKCommon.h"

namespace ade{
    class AdVKDevice;
    class AdVKRenderPass;
    class AdVKImageView;
    class AdVKImage;

    class AdVKFrameBuffer{
    public:
        AdVKFrameBuffer(AdVKDevice *device, AdVKRenderPass *renderPass, const std::vector<std::shared_ptr<AdVKImage>> &images, uint32_t width, uint32_t height);
        ~AdVKFrameBuffer();

        bool ReCreate(const std::vector<std::shared_ptr<AdVKImage>> &images, uint32_t width, uint32_t height);
        VkImageView GetFinalColorAttachment() const { return mImageViews[mFinalColorAttachmentIdx]->GetHandle(); }

        VkFramebuffer GetHandle() const { return mHandle; }
        uint32_t GetWidth() const { return mWidth; }
        uint32_t GetHeight() const { return mHeight; }
    private:
        VkFramebuffer mHandle = VK_NULL_HANDLE;
        AdVKDevice *mDevice;
        AdVKRenderPass *mRenderPass;
        uint32_t mWidth;
        uint32_t mHeight;
        std::vector<std::shared_ptr<AdVKImage>> mImages;
        std::vector<std::shared_ptr<AdVKImageView>> mImageViews;

        uint32_t mFinalColorAttachmentIdx;
    };
}

#endif