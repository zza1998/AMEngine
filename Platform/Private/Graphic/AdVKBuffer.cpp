#include "Graphic/AdVKBuffer.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKCommandBuffer.h"

namespace ade{
    AdVKBuffer::AdVKBuffer(AdVKDevice *device, VkBufferUsageFlags usage, size_t size, void *data, bool bHostVisible)
                    : mDevice(device), mSize(size), bHostVisible(bHostVisible)  {
        CreateBuffer(usage, data);
    }

    AdVKBuffer::~AdVKBuffer() {
        VK_D(Buffer, mDevice->GetHandle(), mHandle);
        VK_F(mDevice->GetHandle(), mMemory);
    }

    void AdVKBuffer::CreateBuffer(VkBufferUsageFlags usage, void *data) {
        if(bHostVisible){
            CreateBufferInternal(mDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, usage, mSize, &mHandle, &mMemory);
            WriteData(data);
        } else {
            VkBuffer stageBuffer;
            VkDeviceMemory stageMemory;
            CreateBufferInternal(mDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT, mSize, &stageBuffer, &stageMemory);

            void *mapping;
            CALL_VK(vkMapMemory(mDevice->GetHandle(), stageMemory, 0, VK_WHOLE_SIZE, 0, &mapping));
            memcpy(mapping, data, mSize);
            vkUnmapMemory(mDevice->GetHandle(), stageMemory);
            CreateBufferInternal(mDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, mSize, &mHandle, &mMemory);

            // copy
            CopyToBuffer(mDevice, stageBuffer, mHandle, mSize);

            VK_D(Buffer, mDevice->GetHandle(), stageBuffer);
            VK_F(mDevice->GetHandle(), stageMemory);
        }
    }

    void AdVKBuffer::CreateBufferInternal(AdVKDevice *device, VkMemoryPropertyFlags memProps, VkBufferUsageFlags usage, size_t size, VkBuffer *outBuffer, VkDeviceMemory *outMemory) {
        VkBufferCreateInfo bufferInfo = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .size = size,
                .usage = usage,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 0,
                .pQueueFamilyIndices = nullptr,
        };
        CALL_VK(vkCreateBuffer(device->GetHandle(), &bufferInfo, nullptr, outBuffer));
        // allocate memory
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(device->GetHandle(), *outBuffer, &memReqs);

        VkMemoryAllocateInfo allocateInfo = {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .pNext = nullptr,
                .allocationSize = memReqs.size,
                .memoryTypeIndex = static_cast<uint32_t>(device->GetMemoryIndex(memProps, memReqs.memoryTypeBits))
        };
        CALL_VK(vkAllocateMemory(device->GetHandle(), &allocateInfo, nullptr, outMemory));
        CALL_VK(vkBindBufferMemory(device->GetHandle(), *outBuffer, *outMemory, 0));
    }

    void AdVKBuffer::CopyToBuffer(AdVKDevice *device, VkBuffer srcBuffer, VkBuffer dstBuffer, size_t size) {
        VkCommandBuffer cmdBuffer = device->CreateAndBeginOneCmdBuffer();
        VkBufferCopy bufferCopy = {
                .srcOffset = 0,
                .dstOffset = 0,
                .size = size
        };
        vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
        device->SubmitOneCmdBuffer(cmdBuffer);
    }

    VkResult AdVKBuffer::WriteData(void *data) {
        if(data && bHostVisible){
            void *mapping;
            VkResult ret = vkMapMemory(mDevice->GetHandle(), mMemory, 0, VK_WHOLE_SIZE, 0, &mapping);
            memcpy(mapping, data, mSize);
            vkUnmapMemory(mDevice->GetHandle(), mMemory);
            return ret;
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }
}