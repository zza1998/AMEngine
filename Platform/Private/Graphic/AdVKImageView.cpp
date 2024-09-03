#include "Graphic/AdVKImageView.h"
#include "Graphic/AdVKDevice.h"

namespace ade{
    AdVKImageView::AdVKImageView(AdVKDevice *device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) : mDevice(device) {
        VkImageViewCreateInfo imageViewInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = format,
                .components = {
                        VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange = {
                        .aspectMask = aspectFlags,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                }
        };
        CALL_VK(vkCreateImageView(device->GetHandle(), &imageViewInfo, nullptr, &mHandle));
    }

    AdVKImageView::~AdVKImageView() {
        VK_D(ImageView, mDevice->GetHandle(), mHandle);
    }
}