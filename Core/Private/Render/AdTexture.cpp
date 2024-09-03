#include "Render/AdTexture.h"

#include "AdApplication.h"
#include "Render/AdRenderContext.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKImage.h"
#include "Graphic/AdVKImageView.h"
#include "Graphic/AdVKBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace ade{
    AdTexture::AdTexture(const std::string &filePath) {
        int numChannel;
        uint8_t *data = stbi_load(filePath.c_str(), reinterpret_cast<int *>(&mWidth), reinterpret_cast<int *>(&mHeight), &numChannel, STBI_rgb_alpha);
        if(!data){
            LOG_E("Can not load this image: {0}", filePath);
            return;
        }

        mFormat = VK_FORMAT_R8G8B8A8_UNORM;
        size_t size = sizeof(uint8_t) * 4 * mWidth * mHeight;
        CreateImage(size, data);

        stbi_image_free(data);
    }

    AdTexture::AdTexture(uint32_t width, uint32_t height, RGBAColor *pixels) : mWidth(width), mHeight(height) {
        mFormat = VK_FORMAT_R8G8B8A8_UNORM;
        size_t size = sizeof(uint8_t) * 4 * mWidth * mHeight;
        CreateImage(size, pixels);
    }

    AdTexture::~AdTexture() {
        mImageView.reset();
        mImage.reset();
    }

    void AdTexture::CreateImage(size_t size, void *data) {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        mImage = std::make_shared<AdVKImage>(device, VkExtent3D{ mWidth, mHeight, 1}, mFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT);
        mImageView = std::make_shared<AdVKImageView>(device, mImage->GetHandle(), mFormat, VK_IMAGE_ASPECT_COLOR_BIT);

        // copy data to buffer
        std::shared_ptr<AdVKBuffer> stageBuffer = std::make_shared<AdVKBuffer>(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, data, true);

        // copy buffer to image
        VkCommandBuffer cmdBuffer = device->CreateAndBeginOneCmdBuffer();
        AdVKImage::TransitionLayout(cmdBuffer, mImage->GetHandle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        mImage->CopyFromBuffer(cmdBuffer, stageBuffer.get());
        AdVKImage::TransitionLayout(cmdBuffer, mImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        device->SubmitOneCmdBuffer(cmdBuffer);
        stageBuffer.reset();
    }
}