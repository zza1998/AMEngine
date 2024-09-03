#ifndef ADRENDETARGET_H
#define ADRENDETARGET_H

#include "Graphic/AdVKFrameBuffer.h"
#include "Render/AdRenderContext.h"
#include "ECS/System/AdMaterialSystem.h"
#include "ECS/AdEntity.h"

namespace ade{
    class AdRenderTarget{
    public:
        AdRenderTarget(AdVKRenderPass *renderPass);
        AdRenderTarget(AdVKRenderPass *renderPass, uint32_t bufferCount, VkExtent2D extent);
        ~AdRenderTarget();

        void Begin(VkCommandBuffer cmdBuffer);
        void End(VkCommandBuffer cmdBuffer);

        AdVKRenderPass *GetRenderPass() const { return mRenderPass; }
        AdVKFrameBuffer *GetFrameBuffer() const { return mFrameBuffers[mCurrentBufferIdx].get(); }

        void SetExtent(const VkExtent2D &extent);
        void SetBufferCount(uint32_t bufferCount);

        void SetColorClearValue(VkClearColorValue colorClearValue);
        void SetColorClearValue(uint32_t attachmentIndex, VkClearColorValue colorClearValue);
        void SetDepthStencilClearValue(VkClearDepthStencilValue depthStencilValue);
        void SetDepthStencilClearValue(uint32_t attachmentIndex, VkClearDepthStencilValue depthStencilValue);

        template<typename T, typename... Args>
        void AddMaterialSystem(Args&&... args) {
            std::shared_ptr<AdMaterialSystem> system = std::make_shared<T>(std::forward<Args>(args)...);
            system->OnInit(mRenderPass);
            mMaterialSystemList.push_back(system);
        }

        void RenderMaterialSystems(VkCommandBuffer cmdBuffer) {
            for (auto &item: mMaterialSystemList){
                item->OnRender(cmdBuffer, this);
            }
        }

        void SetCamera(AdEntity *camera) { mCamera = camera; }
        AdEntity *GetCamera() const { return mCamera; }
    private:
        void Init();
        void ReCreate();

        std::vector<std::shared_ptr<AdVKFrameBuffer>> mFrameBuffers;

        AdVKRenderPass *mRenderPass;
        std::vector<VkClearValue> mClearValues;
        uint32_t mBufferCount;
        uint32_t mCurrentBufferIdx = 0;
        VkExtent2D mExtent;

        bool bSwapchainTarget = false;
        bool bBeginTarget = false;

        std::vector<std::shared_ptr<AdMaterialSystem>> mMaterialSystemList;
        AdEntity *mCamera= nullptr;

        bool bShouldUpdate = false;
    };
}

#endif