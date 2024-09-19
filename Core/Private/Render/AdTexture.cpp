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

    AdCubeTexture::~AdCubeTexture() {
        mImageView.reset();
        mImage.reset();
    }
    AdCubeTexture::AdCubeTexture(const std::vector<std::string>& imageData, VkFormat imageFormat) {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        if(imageData.size() != 6) {
            return;
        }
        // transfer image
        void* allImages[6];
        int numChannel;
        mFormat = VK_FORMAT_R8G8B8A8_UNORM;
        int i = 0;
        for(auto sImage:imageData) {
            uint8_t *data = stbi_load(sImage.c_str(), reinterpret_cast<int *>(&mWidth), reinterpret_cast<int *>(&mHeight), &numChannel, STBI_rgb_alpha);
            if(!data){
                LOG_E("Can not load this image: {0}", sImage);
                return;
            }
            allImages[i++]=data;
        }

        // Step 1: Create the Vulkan Image for the cubemap
        // Step 2: Allocate and bind memory for the cubemap image
        mImage = std::make_shared<AdVKImage>(device,VkExtent3D{mWidth,mHeight,1},mFormat,1);

        // Step 3: Create a staging buffer and copy image data to it
        VkDeviceSize imageSize = sizeof(uint8_t) * mWidth * mHeight * 4; // 4 bytes per pixel (RGBA)
        VkDeviceSize bufferSize = imageSize * 6;
        std::shared_ptr<AdVKBuffer> stageBuffer = std::make_shared<AdVKBuffer>(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, bufferSize, allImages, true);


        // Step 4: Copy data from the staging buffer to the cubemap image
        VkCommandBuffer cmdBuffer = device->CreateAndBeginOneCmdBuffer();
        AdVKImage::TransitionLayout(cmdBuffer, mImage->GetHandle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,6);
        for (uint32_t i = 0; i < 6; ++i) {
            VkBufferImageCopy region = {};
            region.bufferOffset = imageSize * i;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = i; // Index for each face
            region.imageSubresource.layerCount = 1;
            region.imageOffset = {0, 0, 0};
            region.imageExtent = { mWidth, mHeight, 1 };

            vkCmdCopyBufferToImage(cmdBuffer, stageBuffer->GetHandle(), mImage->GetHandle(),
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        }
        device->SubmitOneCmdBuffer(cmdBuffer);
        AdVKImage::TransitionLayout(cmdBuffer, mImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 6);


        // Step 5: Create the image view for the cubemap
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mImage->GetHandle();
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        viewInfo.format = imageFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 6;

        vkCreateImageView(device->GetHandle(), &viewInfo, nullptr,mImageView->GetHandlePtr());

        // Step 7: Clean up the staging buffer
        stageBuffer.reset();
        for(auto img:allImages) {
            stbi_image_free(img);
        }
    }

}