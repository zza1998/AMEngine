#include "Window/AdEditorMainWindow.h"
#include "AdEngine.h"
#include "AdApplication.h"
#include "AdEditorApp.h"
#include "Render/AdRenderTarget.h"
#include "ECS/AdNode.h"
#include "ECS/Component/AdCameraComponent.h"
#include "ECS/System/AdBaseRenderSystem.h"

#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_internal.h"

namespace ade{
    void AdEditorMainWindow::Draw(bool *pOpen) {
        if(pOpen && *pOpen){
            if(bOpens.bOpenHeaderBar){
                ShowAppMainMenuBar();
            }

            ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_PassthruCentralNode;
            if(!bReDocking){
                dockNodeFlags |= ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoDockingOverCentralNode | ImGuiDockNodeFlags_NoDockingSplit;
            }
            mViewportDockId = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockNodeFlags);

            if(bOpens.bOpenAppWindow){
                if(ImGui::Begin("Application", &bOpens.bOpenAppWindow)){
                    mApplicationWidget.Draw();
                }
                ImGui::End();
            }

            if(bOpens.bOpenDemoWindow){
                ImGui::ShowDemoWindow(&bOpens.bOpenDemoWindow);
                ImPlot::ShowDemoWindow(&bOpens.bOpenDemoWindow);
            }

            if(bOpens.bOpenContentWindow){
                mEditorContentWindow.Draw(&bOpens.bOpenContentWindow);
            }

            if(bOpens.bOpenLayerWindow){
                mEditorLayerWindow.Draw(&bOpens.bOpenLayerWindow);
            }

            if(bOpens.bOpenPropertyWindow){
                mEditorPropertyWindow.Draw(&bOpens.bOpenPropertyWindow);
            }
        }
    }

    void AdEditorMainWindow::AddViewportWindow(AdRenderTarget *renderTarget) {
        if(mViewportWindows.find(renderTarget) == mViewportWindows.end()){
            std::string viewportName = std::to_string(mViewportWindows.size());
            if(renderTarget->GetCamera()){
                viewportName = renderTarget->GetCamera()->GetName();
            }
            mViewportWindows.insert({ renderTarget, std::make_shared<AdEditorViewportWindow>(renderTarget, viewportName) });
        }
    }

    void AdEditorMainWindow::RemoveViewportWindow(AdRenderTarget *renderTarget) {
        auto it = mViewportWindows.find(renderTarget);
        if(it != mViewportWindows.end()){
            mViewportWindows.erase(it);
        }
    }

    void AdEditorMainWindow::DrawViewportWindow(AdRenderTarget *renderTarget, float deltaTime) {
        if(mViewportWindows.find(renderTarget) != mViewportWindows.end()){
            mViewportWindows[renderTarget]->Draw(nullptr, mViewportDockId, deltaTime);
        }
    }

    void AdEditorMainWindow::ShowAppMainMenuBar(){
        if (ImGui::BeginMainMenuBar()){
            if (ImGui::BeginMenu("File")){
                ShowExampleMenuFile();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")){
                if (ImGui::BeginMenu("Theme")) {
                    if(ImGui::MenuItem("Dark", "")){
                        ImGui::StyleColorsDark();
                    }
                    if(ImGui::MenuItem("Light", "")){
                        ImGui::StyleColorsLight();
                    }
                    if(ImGui::MenuItem("Classic", "")){
                        ImGui::StyleColorsLight();
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Multi Sampler[Restart]")) {

                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Undo", "CTRL+Z")) {

                }
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {

                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window")){
                if (ImGui::MenuItem("Can docking", "", &bReDocking)) {

                }
                ImGui::MenuItem("Demo Window", "", &bOpens.bOpenDemoWindow);
                ImGui::MenuItem("Application",  "", &bOpens.bOpenAppWindow);
                ImGui::MenuItem("Layer", "", &bOpens.bOpenLayerWindow);
                ImGui::MenuItem("Content",  "", &bOpens.bOpenContentWindow);
                ImGui::MenuItem("Header Bars",  "", &bOpens.bOpenHeaderBar);
                ImGui::Separator();
                if(ImGui::BeginMenu("Add Viewport Window", mViewportWindows.size() < 5)){
                    AdScene *scene = AdApplication::GetContext()->scene;
                    if(scene){
                        scene->GetEcsRegistry().view<AdPerspCameraComponent>().each([scene](const auto &e, AdPerspCameraComponent &cameraComp){
                            AdNode *node = scene->GetNode(e);
                            if(ImGui::MenuItem(node->GetName().c_str(), nullptr, cameraComp.mPreviewIndex > -1)){
                                LOG_T("{0}", node->GetName());
                                AdEditorApp *app = dynamic_cast<AdEditorApp *>(AdApplication::GetContext()->app);
                                if(cameraComp.mPreviewIndex > -1){
                                    app->RemoveViewportWindow(cameraComp.mPreviewIndex);
                                    cameraComp.mPreviewIndex = -1;
                                } else {
                                    app->AddViewportWindow(app->GetSceneRenderPass(), node, reinterpret_cast<uint32_t *>(&cameraComp.mPreviewIndex));
                                }
                            }
                        });
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add")){
                if(ImGui::MenuItem("Empty Entity")){
                }
                if(ImGui::MenuItem("Skybox")){
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Cube")){
                }
                if(ImGui::MenuItem("Cube2")){
                }

                ImGui::Separator();
                if(ImGui::MenuItem("Orth Camera")){
                }
                if(ImGui::MenuItem("Around Persp Camera")){
                }

                if(ImGui::MenuItem("1st Person Persp Camera")){
                }

                ImGui::Separator();
                if(ImGui::MenuItem("Direct Light")){
                }
                if(ImGui::MenuItem("Point Light")){
                }
                if(ImGui::MenuItem("Spot Light")){
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

//        if (BeginToolBars("ToolBar_Down", ImGuiDir_Down)) {
//            ImGui::MenuItem(ade::icon::File, "");
//            ImGui::MenuItem(ade::icon::AddressCard);
//            ImGui::MenuItem(ade::icon::Baby);
//            ImGui::MenuItem(ade::icon::Database);
//            EndToolBars(ImGuiDir_Down);
//        }
//
//        if (BeginToolBars("ToolBar_Left", ImGuiDir_Left)) {
//            ImGui::MenuItem(ade::icon::Cube, "");
//            ImGui::MenuItem(ade::icon::File);
//            ImGui::MenuItem(ade::icon::File);
//            ImGui::MenuItem(ade::icon::File);
//            EndToolBars(ImGuiDir_Left);
//        }
//
//        if (BeginToolBars("ToolBar_Right", ImGuiDir_Right)) {
//            ImGui::MenuItem(ade::icon::Search, "");
//            ImGui::MenuItem(ade::icon::Bug, "");
//            EndToolBars(ImGuiDir_Right);
//        }
    }

    void AdEditorMainWindow::ShowExampleMenuFile(){
        if (ImGui::BeginMenu("New")) {
            if(ImGui::MenuItem("Empty Scene")){

            }
            if(ImGui::MenuItem("Default Scene")){

            }
            if(ImGui::MenuItem("Light Scene")){

            }
            ImGui::EndMenu();
        }
    }

    bool AdEditorMainWindow::BeginToolBars(const char* name, ImGuiDir dir) {
        ImGuiViewportP* mainViewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
        ImGui::SetCurrentViewport(NULL, mainViewport);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        float height = ImGui::GetFrameHeight();
        if(dir == ImGuiDir_Left || dir == ImGuiDir_Right){
            height *= 1.5f;
        }
        bool is_open = ImGui::BeginViewportSideBar(name, mainViewport, dir, height, window_flags);

        if (is_open){
            if(dir == ImGuiDir_Up || dir == ImGuiDir_Down){
                ImGui::BeginMenuBar();
            }
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 10.0f, 30.0f });
        } else {
            ImGui::End();
        }
        return is_open;
    }

    void AdEditorMainWindow::EndToolBars(ImGuiDir dir) {
        ImGui::PopStyleVar();
        if(dir == ImGuiDir_Up || dir == ImGuiDir_Down){
            ImGui::EndMenuBar();
        }

        ImGuiContext& g = *GImGui;
        if (g.CurrentWindow == g.NavWindow && g.NavLayer == ImGuiNavLayer_Main && !g.NavAnyRequest)
            ImGui::FocusTopMostWindowUnderOne(g.NavWindow, NULL, NULL,
                                              ImGuiFocusRequestFlags_UnlessBelowModal
                                              | ImGuiFocusRequestFlags_RestoreFocusedChild);

        ImGui::End();
    }
}