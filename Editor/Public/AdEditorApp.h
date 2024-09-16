#ifndef AD_EDITOR_APP_H
#define AD_EDITOR_APP_H

#include "AdApplication.h"
#include "Graphic/AdVKCommon.h"
#include "Windows/AdEditorMainWindow.h"

namespace ade{
    class AdRenderer;
    class AdRenderPass;
    class AdRenderTarget;
    class AdVKCommandPool;
    class AdVKCommandBuffer;
    class AdVKDescriptorPool;

    class AdEditorApp : public AdApplication{
    public:
        AdEditorApp() = default;
        std::shared_ptr<AdRenderTarget> AddViewportWindow(AdVKRenderPass *renderPass, uint32_t *outIndex = nullptr);

        void RemoveViewportWindow(uint32_t index);
    protected:
        void OnInit();
        //void OnRenderGui(float deltaTime) override;
        void OnRender();
        void OnDestroy() override;

        std::vector<VkCommandBuffer> mGuiCmdBuffers;
        std::shared_ptr<AdRenderTarget> mGuiRenderTarget;
        std::vector<std::shared_ptr<AdRenderTarget>> mSceneRenderTargets;
        int32_t imageIndex;
    private:
        void InitImGui();
        void MergeIconFonts();


        std::shared_ptr<ade::AdRenderer> mRenderer;
        std::shared_ptr<AdVKDescriptorPool> mGuiDescriptorPool;
        std::shared_ptr<AdVKCommandPool> mGuiCmdPool;


        std::shared_ptr<AdVKRenderPass> mGuiRenderPass;


        std::vector<VkCommandBuffer> mSceneCmdBuffers;

        AdEditorMainWindow mMainWindow;
        bool bOpenMainWindow = true;

        friend class AdApplicationWidget;


    };
}
#endif