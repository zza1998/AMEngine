#ifndef AD_EDITOR_APP_H
#define AD_EDITOR_APP_H

#include "AdApplication.h"
#include "Graphic/AdVKCommon.h"
#include "Window/AdEditorMainWindow.h"

namespace ade{
    class AdRenderPass;
    class AdRenderTarget;
    class AdVKCommandPool;
    class AdVKCommandBuffer;
    class AdVKDescriptorPool;

    class AdEditorApp : public AdApplication{
    public:
        AdEditorApp();
        AdRenderTarget* AddViewportWindow(AdRenderPass *renderPass, AdNode *defaultCamera, uint32_t *outIndex = nullptr);
        void RemoveViewportWindow(uint32_t index);
    protected:
        void OnInit() override;
        void OnRenderGui(float deltaTime) override;
        void OnRender() override;
        void OnDestroy() override;
    private:
        void InitImGui();
        void MergeIconFonts();

        std::shared_ptr<AdVKDescriptorPool> mGuiDescriptorPool;
        std::shared_ptr<AdVKCommandPool> mGuiCmdPool;
        std::vector<std::shared_ptr<AdVKCommandBuffer>> mGuiCmdBuffers;

        std::shared_ptr<AdRenderPass> mGuiRenderPass;
        std::shared_ptr<AdRenderTarget> mGuiRenderTarget;
        std::vector<std::shared_ptr<AdRenderTarget>> mSceneRenderTargets;
        std::vector<std::shared_ptr<AdVKCommandBuffer>> mSceneCmdBuffers;

        AdEditorMainWindow mMainWindow;
        bool bOpenMainWindow = true;

        friend class AdApplicationWidget;
    };
}
#endif