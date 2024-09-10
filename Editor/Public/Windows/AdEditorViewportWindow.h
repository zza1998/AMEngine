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

        void Draw(bool *pOpen, ImGuiID dockId);
    private:
        void RecreateRenderTarget(VkExtent2D newExtent);
        void ProcessInput(float deltaTime);
        void ShowToolBars(ImVec2 workPos);

        std::string mName;
        AdRenderTarget *mRenderTarget;
        std::vector<VkDescriptorSet> mDescriptorSets;

        struct{
            bool bTransformWidgetIsUsing = false;
            bool bStartMouseDrag = false;
            bool bFirstMouseDrag = true;
            ImVec2 mousePose;
        } mInput;
    };
}

#endif