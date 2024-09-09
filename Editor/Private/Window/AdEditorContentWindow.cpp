#include "Window/AdEditorContentWindow.h"
#include "imgui/imgui.h"
#include "AdLog.h"

namespace ade{
    void AdEditorContentWindow::Draw(bool *pOpen) {
        if(pOpen && *pOpen){
            ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Content", pOpen)){
                mFileBrowserWidget.Draw([](const std::filesystem::path &path){
                    LOG_T("Select path: {0}", path.string());
                });
            }
            ImGui::End();
        }
    }
}