//
// Created by zhou on 2024/9/26.
//

#include "Windows/AdEditorSettingWindow.h"
#include "AdApplication.h"
#include "ECS/AdEntity.h"
#include "ECS/AdComponent.h"
#include "AdLog.h"
#include "Gui/Widget/AdWidget.h"
#include "imgui/imgui.h"
namespace ade{

 void AdEditorSettingWindow::Draw(bool *pOpen) {
        if(ImGui::Begin("GlobalSetting", pOpen)){
            AdAppContext *appContext = AdApplication::GetAppContext();

            ImGui::Text("AmEngine Settings");
        }
        ImGui::End();
    }

}