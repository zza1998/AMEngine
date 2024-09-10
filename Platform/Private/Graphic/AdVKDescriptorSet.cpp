#include "Graphic/AdVKDescriptorSet.h"
#include "Graphic/AdVKDevice.h"

namespace ade{
    AdVKDescriptorSetLayout::AdVKDescriptorSetLayout(AdVKDevice *device, const std::vector<VkDescriptorSetLayoutBinding> &bindings) : mDevice(device) {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .bindingCount = static_cast<uint32_t>(bindings.size()),
                .pBindings = bindings.data()
        };
        CALL_VK(vkCreateDescriptorSetLayout(mDevice->GetHandle(), &descriptorSetLayoutInfo, nullptr, &mHandle));
    }

    AdVKDescriptorSetLayout::~AdVKDescriptorSetLayout() {
        VK_D(DescriptorSetLayout, mDevice->GetHandle(), mHandle);
    }

    AdVKDescriptorPool::AdVKDescriptorPool(AdVKDevice *device, uint32_t maxSets, const std::vector<VkDescriptorPoolSize> &poolSizes,const VkDescriptorPoolCreateFlags flags) : mDevice(device) {
        VkDescriptorPoolCreateInfo descriptorPoolInfo = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = flags,
                .maxSets = maxSets,
                .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
                .pPoolSizes = poolSizes.data()
        };
        CALL_VK(vkCreateDescriptorPool(mDevice->GetHandle(), &descriptorPoolInfo, nullptr, &mHandle));
    }

    AdVKDescriptorPool::~AdVKDescriptorPool() {
        VK_D(DescriptorPool, mDevice->GetHandle(), mHandle);
    }

    std::vector<VkDescriptorSet> AdVKDescriptorPool::AllocateDescriptorSet(AdVKDescriptorSetLayout *setLayout, uint32_t count) {
        std::vector<VkDescriptorSet> descriptorSets(count);
        std::vector<VkDescriptorSetLayout> setLayouts(count);
        for(int i = 0; i < count; i++){
            setLayouts[i] = setLayout->GetHandle();
        }
        VkDescriptorSetAllocateInfo allocateInfo = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .pNext = nullptr,
                .descriptorPool = mHandle,
                .descriptorSetCount = count,
                .pSetLayouts = setLayouts.data()
        };
        VkResult ret = vkAllocateDescriptorSets(mDevice->GetHandle(), &allocateInfo, descriptorSets.data());
        CALL_VK(ret);
        if(ret != VK_SUCCESS){
            descriptorSets.clear();
        }
        return descriptorSets;
    }
}