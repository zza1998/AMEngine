#ifndef AD_DESCRIPTOR_SET_H
#define AD_DESCRIPTOR_SET_H

#include "AdVKCommon.h"

namespace ade{
    class AdVKDevice;

    class AdVKDescriptorSetLayout{
    public:
        //AdVKDescriptorSetLayout(AdVKDescriptorSetLayout && other);
        AdVKDescriptorSetLayout(AdVKDevice *device, const std::vector<VkDescriptorSetLayoutBinding> &bindings);
        ~AdVKDescriptorSetLayout();

        VkDescriptorSetLayout GetHandle() const { return mHandle; }
    private:
        VkDescriptorSetLayout mHandle = VK_NULL_HANDLE;

        AdVKDevice *mDevice;
    };

    class AdVKDescriptorPool{
    public:
        AdVKDescriptorPool(AdVKDevice *device, uint32_t maxSets, const std::vector<VkDescriptorPoolSize> &poolSizes, const VkDescriptorPoolCreateFlags flags = 0);
        ~AdVKDescriptorPool();


        VkDescriptorPool GetHandle() const { return mHandle; }
        std::vector<VkDescriptorSet> AllocateDescriptorSet(AdVKDescriptorSetLayout *setLayout, uint32_t count);
    private:
        VkDescriptorPool mHandle = VK_NULL_HANDLE;

        AdVKDevice *mDevice;
    };

    class DescriptorSetWriter{
    public:
        static VkDescriptorBufferInfo BuildBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range){
            VkDescriptorBufferInfo bufferInfo{
                .buffer = buffer,
                .offset = offset,
                .range = range
            };
            return bufferInfo;
        }

        static VkDescriptorImageInfo BuildImageInfo(VkSampler sampler, VkImageView imageView){
            VkDescriptorImageInfo imageInfo{
                .sampler = sampler,
                .imageView = imageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };
            return imageInfo;
        }

        static VkWriteDescriptorSet WriteBuffer(VkDescriptorSet dstSet, uint32_t dstBinding, VkDescriptorType descriptorType, const VkDescriptorBufferInfo *pBufferInfo){
            VkWriteDescriptorSet writeDescriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = dstSet,
                .dstBinding = dstBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = descriptorType,
                .pBufferInfo = pBufferInfo
            };
            return writeDescriptorSet;
        }
        static VkWriteDescriptorSet WriteImage(VkDescriptorSet dstSet, uint32_t dstBinding, VkDescriptorType descriptorType, const VkDescriptorImageInfo *pImageInfo){
            VkWriteDescriptorSet writeDescriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = dstSet,
                .dstBinding = dstBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = descriptorType,
                .pImageInfo = pImageInfo
            };
            return writeDescriptorSet;
        }
        static void UpdateDescriptorSets(VkDevice device, const std::vector<VkWriteDescriptorSet> &writes) {
            vkUpdateDescriptorSets(device, writes.size(), writes.data(), 0, nullptr);
        }
    };
}

#endif