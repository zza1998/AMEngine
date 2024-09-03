#include "Graphic/AdVKCommandBuffer.h"
#include "Graphic/AdVKDevice.h"

namespace ade{
    AdVKCommandPool::AdVKCommandPool(AdVKDevice *device, uint32_t queueFamilyIndex) : mDevice(device) {
        VkCommandPoolCreateInfo commandPoolInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = queueFamilyIndex
        };
        CALL_VK(vkCreateCommandPool(mDevice->GetHandle(), &commandPoolInfo, nullptr, &mHandle));
        LOG_T("Create command pool : {0}", (void*)mHandle);
    }

    AdVKCommandPool::~AdVKCommandPool() {
        VK_D(CommandPool, mDevice->GetHandle(), mHandle);
    }

    std::vector<VkCommandBuffer> AdVKCommandPool::AllocateCommandBuffer(uint32_t count) const {
        std::vector<VkCommandBuffer> cmdBuffers(count);
        VkCommandBufferAllocateInfo allocateInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = mHandle,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = count
        };
        CALL_VK(vkAllocateCommandBuffers(mDevice->GetHandle(), &allocateInfo, cmdBuffers.data()));
        return cmdBuffers;
    }

    VkCommandBuffer AdVKCommandPool::AllocateOneCommandBuffer() const {
        std::vector<VkCommandBuffer> cmdBuffers = AllocateCommandBuffer(1);
        return cmdBuffers[0];
    }

    void AdVKCommandPool::BeginCommandBuffer(VkCommandBuffer cmdBuffer) {
        CALL_VK(vkResetCommandBuffer(cmdBuffer, 0));
        VkCommandBufferBeginInfo beginInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = 0,
                .pInheritanceInfo = nullptr
        };
        CALL_VK(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
    }

    void AdVKCommandPool::EndCommandBuffer(VkCommandBuffer cmdBuffer) {
        CALL_VK(vkEndCommandBuffer(cmdBuffer));
    }
}