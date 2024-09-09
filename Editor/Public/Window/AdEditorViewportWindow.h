#ifndef ADEDITORVIEWPORTWINDOW_H
#define ADEDITORVIEWPORTWINDOW_H

#include "Graphic/AdVKCommon.h"
#include "Render/AdRenderTarget.h"
#include "Gui/Widget/AdTransformWidget.h"

namespace ade{
    class AdEditorViewportWindow{
    public:
        AdEditorViewportWindow(AdRenderTarget *renderTarget, const std::string &name = "");
        ~AdEditorViewportWindow();

        void Draw(bool *pOpen, ImGuiID dockId, float deltaTime);
    private:
        void RecreateRenderTarget(VkExtent2D newExtent);
        void ProcessInput(float deltaTime);
        void ShowToolBars(ImVec2 workPos);

        std::string mName;
        AdRenderTarget *mRenderTarget;
        VkSampler mSampler = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> mDescriptorSets;
        AdTransformWidget mTransformWidget;

        struct{
            bool bTransformWidgetIsUsing = false;
            bool bStartMouseDrag = false;
            bool bFirstMouseDrag = true;
            ImVec2 mousePose;
        } mInput;
    };
}

#endif