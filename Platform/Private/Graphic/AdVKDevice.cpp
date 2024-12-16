#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKCommandBuffer.h"

namespace ade{
    const DeviceFeature requestedExtensions[] = {
            { VK_KHR_SWAPCHAIN_EXTENSION_NAME, true },
#ifdef AD_ENGINE_PLATFORM_WIN32
#elif AD_ENGINE_PLATFORM_MACOS
            { "VK_KHR_portability_subset", true },
#elif AD_ENGINE_PLATFORM_LINUX
#endif
    };

    AdVKDevice::AdVKDevice(AdVKGraphicContext *context, uint32_t graphicQueueCount, uint32_t presentQueueCount, uint32_t computeQueueCount, const AdVkSettings &settings) : mContext(context), mSettings(settings) {
        if(!context){
            LOG_E("Must create a vulkan graphic context before create device.");
            return;
        }

        QueueFamilyInfo graphicQueueFamilyInfo = context->GetGraphicQueueFamilyInfo();
        QueueFamilyInfo presentQueueFamilyInfo = context->GetPresentQueueFamilyInfo();
        QueueFamilyInfo computeQueueFamilyInfo = context->GetComputeQueueFamilyInfo();
        if(graphicQueueCount > graphicQueueFamilyInfo.queueCount){
            LOG_E("this graphic queue family has {0} queue, but request {1}", graphicQueueFamilyInfo.queueCount, graphicQueueCount);
            return;
        }
        if(presentQueueCount > presentQueueFamilyInfo.queueCount){
            LOG_E("this present queue family has {0} queue, but request {1}", presentQueueFamilyInfo.queueCount, presentQueueCount);
            return;
        }
        if(computeQueueCount > computeQueueFamilyInfo.queueCount){
            LOG_E("this compute queue family has {0} queue, but request {1}", computeQueueFamilyInfo.queueCount, computeQueueCount);
            return;
        }
        std::vector<float> graphicQueuePriorities(graphicQueueCount, 0.f);
        std::vector<float> presentQueuePriorities(presentQueueCount, 1.f);

        bool bSameQueueFamilyIndex = context->IsSameGraphicPresentQueueFamily();
        uint32_t sameQueueCount = graphicQueueCount;
        if(bSameQueueFamilyIndex){
            sameQueueCount += presentQueueCount;
            if(sameQueueCount > graphicQueueFamilyInfo.queueCount){
                sameQueueCount = graphicQueueFamilyInfo.queueCount;
            }
            graphicQueuePriorities.insert(graphicQueuePriorities.end(), presentQueuePriorities.begin(), presentQueuePriorities.end());
        }

        VkDeviceQueueCreateInfo queueInfos[2] = {
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .queueFamilyIndex = static_cast<uint32_t>(graphicQueueFamilyInfo.queueFamilyIndex),
                    .queueCount = sameQueueCount,
                    .pQueuePriorities = graphicQueuePriorities.data()
                }
        };

        if(!bSameQueueFamilyIndex){
            queueInfos[1] = {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .queueFamilyIndex = static_cast<uint32_t>(presentQueueFamilyInfo.queueFamilyIndex),
                    .queueCount = presentQueueCount,
                    .pQueuePriorities = presentQueuePriorities.data()
            };
        }

        uint32_t availableExtensionCount;
        CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhyDevice(), "", &availableExtensionCount, nullptr));
        VkExtensionProperties *availableExtensions = new VkExtensionProperties[availableExtensionCount];
        CALL_VK(vkEnumerateDeviceExtensionProperties(context->GetPhyDevice(), "", &availableExtensionCount, availableExtensions));
        uint32_t enableExtensionCount;
        const char *enableExtensions[32];
        if(!checkDeviceFeatures("Device Extension", true, availableExtensionCount, availableExtensions,
                ARRAY_SIZE(requestedExtensions), requestedExtensions, &enableExtensionCount, enableExtensions)){
            return;
        }

        VkDeviceCreateInfo deviceInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueCreateInfoCount = static_cast<uint32_t>(bSameQueueFamilyIndex ? 1 : 2),
                .pQueueCreateInfos = queueInfos,
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = enableExtensionCount,
                .ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr,
                .pEnabledFeatures = nullptr
        };
        CALL_VK(vkCreateDevice(context->GetPhyDevice(), &deviceInfo, nullptr, &mHandle));
        LOG_T("VkDevice: {0}", (void*)mHandle);

        for(int i = 0; i < graphicQueueCount; i++){
            VkQueue queue;
            vkGetDeviceQueue(mHandle, graphicQueueFamilyInfo.queueFamilyIndex, i, &queue);
            mGraphicQueues.push_back(std::make_shared<AdVKQueue>(graphicQueueFamilyInfo.queueFamilyIndex, i, queue, false));
        }
        for(int i = 0; i < presentQueueCount; i++){
            VkQueue queue;
            vkGetDeviceQueue(mHandle, presentQueueFamilyInfo.queueFamilyIndex, i, &queue);
            mPresentQueues.push_back(std::make_shared<AdVKQueue>(presentQueueFamilyInfo.queueFamilyIndex, i, queue, true));
        }
        for(int i = 0; i < computeQueueCount; i++){
            VkQueue queue;
            vkGetDeviceQueue(mHandle, computeQueueFamilyInfo.queueFamilyIndex, i, &queue);
            mComputeQueues.push_back(std::make_shared<AdVKQueue>(computeQueueFamilyInfo.queueFamilyIndex, i, queue, false));
        }

        // create a pipeline cache
        CreatePipelineCache();

        // create default cmd pool
        CreateDefaultCmdPool();

        // create compute cmd pool
        CreateComputeCmdPool();
    }

    AdVKDevice::~AdVKDevice() {
        vkDeviceWaitIdle(mHandle);
        mDefaultCmdPool = nullptr;
        mComputeCmdPool = nullptr;
        VK_D(PipelineCache, mHandle, mPipelineCache);
        vkDestroyDevice(mHandle, nullptr);
    }

    void AdVKDevice::CreatePipelineCache() {
        VkPipelineCacheCreateInfo pipelineCacheInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0
        };
        CALL_VK(vkCreatePipelineCache(mHandle, &pipelineCacheInfo, nullptr, &mPipelineCache));
    }

    void AdVKDevice::CreateDefaultCmdPool() {
        mDefaultCmdPool = std::make_shared<ade::AdVKCommandPool>(this, mContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
    }

    void AdVKDevice::CreateComputeCmdPool() {
        mComputeCmdPool = std::make_shared<ade::AdVKCommandPool>(this, mContext->GetComputeQueueFamilyInfo().queueFamilyIndex);
    }

    int32_t AdVKDevice::GetMemoryIndex(VkMemoryPropertyFlags memProps, uint32_t memoryTypeBits) const {
        VkPhysicalDeviceMemoryProperties phyDeviceMemProps = mContext->GetPhyDeviceMemProperties();
        if(phyDeviceMemProps.memoryTypeCount == 0){
            LOG_E("Physical device memory type count is 0");
            return -1;
        }
        for(int i = 0; i < phyDeviceMemProps.memoryTypeCount; i++){
            if(memoryTypeBits & (1 << i) && (phyDeviceMemProps.memoryTypes[i].propertyFlags & memProps) == memProps){
                return i;
            }
        }
        LOG_E("Can not find memory type index: type bit: {0}", memoryTypeBits);
        return 0;
    }

    VkCommandBuffer AdVKDevice::CreateAndBeginOneCmdBuffer(bool isGraphic) {
        VkCommandBuffer cmdBuffer;
        if(isGraphic) {
            cmdBuffer = mDefaultCmdPool->AllocateOneCommandBuffer();
            mDefaultCmdPool->BeginCommandBuffer(cmdBuffer);
        }else {
            cmdBuffer = mComputeCmdPool->AllocateOneCommandBuffer();
            mComputeCmdPool->BeginCommandBuffer(cmdBuffer);
        }
        return cmdBuffer;
    }

    void AdVKDevice::SubmitOneCmdBuffer(VkCommandBuffer cmdBuffer,bool isGraphic) {
        AdVKQueue *queue ;
        if(isGraphic) {
            mDefaultCmdPool->EndCommandBuffer(cmdBuffer);
            queue = GetFirstGraphicQueue();
        }else {
            mComputeCmdPool->EndCommandBuffer(cmdBuffer);
            queue = GetFirstComputeQueue();
        }

        queue->Submit({ cmdBuffer });
        queue->WaitIdle();
        // if(isGraphic) {
        //     vkFreeCommandBuffers(mHandle,mDefaultCmdPool->GetHandle(),1,&cmdBuffer);
        // }else {
        //     vkFreeCommandBuffers(mHandle,mComputeCmdPool->GetHandle(),1,&cmdBuffer);
        // }

    }

    VkResult AdVKDevice::CreateSimpleSampler(VkFilter filter, VkSamplerAddressMode addressMode, VkSampler *outSampler,uint16_t mipLevels) {
        VkSamplerCreateInfo samplerInfo = {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = filter,
                .minFilter = filter,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU = addressMode,
                .addressModeV = addressMode,
                .addressModeW = addressMode,
                .mipLodBias = 0,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_NEVER,
                .minLod = 0,
                .maxLod = (float)mipLevels,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE
        };
        return vkCreateSampler(mHandle, &samplerInfo, nullptr, outSampler);
    }
}