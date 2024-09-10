#ifndef AD_EDITOR_MAINWINDOW_H
#define AD_EDITOR_MAINWINDOW_H

#include "AdEditorViewportWindow.h"
#include "AdEditorContentWindow.h"
#include "AdEditorLayerWindow.h"
#include "AdEditorPropertyWindow.h"

#include "imgui/imgui.h"
#include "imgui/implot.h"

namespace ade{
    class AdVKFrameBuffer;

    struct WindowOpens{
        bool bOpenHeaderBar = true;
        bool bOpenAppWindow = true;
        bool bOpenDemoWindow = true;
        bool bOpenContentWindow = true;
        bool bOpenLayerWindow = true;
        bool bOpenPropertyWindow = true;
    };

    class AdEditorMainWindow{
    public:
        void Draw(bool *pOpen);

        void AddViewportWindow(AdRenderTarget *renderTarget);
        void RemoveViewportWindow(AdRenderTarget *renderTarget);
        void DrawViewportWindow(AdRenderTarget *renderTarget, float deltaTime = 0.0f);
    private:
        void ShowAppMainMenuBar();
        void ShowExampleMenuFile();
        bool BeginToolBars(const char* name, ImGuiDir dir);
        void EndToolBars(ImGuiDir dir);

        AdEditorContentWindow mEditorContentWindow;
        AdEditorLayerWindow mEditorLayerWindow;
        AdEditorPropertyWindow mEditorPropertyWindow;
        std::unordered_map<AdRenderTarget*, std::shared_ptr<AdEditorViewportWindow>> mViewportWindows;

        ImGuiID mViewportDockId;
        bool bReDocking = true;

        WindowOpens bOpens;
    };
}

#endif