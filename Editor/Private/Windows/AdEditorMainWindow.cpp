#include "Windows/AdEditorMainWindow.h"
#include "AdEngine.h"
#include "AdApplication.h"
#include "Render/AdRenderTarget.h"
#include "ECS/AdEntity.h"
#include "ECS/Component/AdLookAtCameraComponent.h"

#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_internal.h"
#include "Gui/AdFontAwesomeIcons.h"

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
            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockNodeFlags);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            /*if(ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_UnsavedDocument | ImGuiWindowFlags_MenuBar)){
                AdApplication *app = AdApplication::GetAppContext()->app;
                if(ImGui::BeginMenuBar()){
                    float size = 26.f;
                    uint32_t itemCount = 6;
                    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * itemCount * 0.5f));

                    bool bPushColor = false;
                    if(app->IsPause()){
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.66f, 0.f, 1.f));
                        bPushColor = true;
                    }
                    if(ImGui::Button(app->IsPause() ? ade::icon::Play : ade::icon::Pause, ImVec2(size, size))){
                        if(app->IsPause()){
                            app->Resume();
                        } else {
                            app->Pause();
                        }
                    } ImGui::SameLine();
                    if(bPushColor){
                        ImGui::PopStyleColor();
                    }

                    if(ImGui::MenuItem(ade::icon::Stop)){

                    }

                    ImGui::EndMenuBar();
                }

                mViewportDockId = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(mViewportDockId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_AutoHideTabBar);

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav;
                float size = ImGui::GetTextLineHeight();
                ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - size);
                ImGui::SetNextWindowBgAlpha(0.3f);
                PerformanceParams performanceParams = AdApplication::GetAppContext()->renderCxt->GetPerformanceParams();
                if(ImGui::BeginChild("##toolbar_1", ImVec2(0, 0), ImGuiChildFlags_None, window_flags)){
                    ImGui::SetCursorPosX(ImGui::GetTextLineHeight());
                    ImGuiIO &io = ImGui::GetIO();
                    ImGui::Text("%.2fms/f (%4.0fFPS)", 1000.0f / io.Framerate, io.Framerate);

                    ImGui::SameLine(0, ImGui::GetTextLineHeight());
                    ImGui::Text("Vertices : %d, %lu", io.MetricsRenderVertices, performanceParams.vertexCount);

                    ImGui::SameLine(0, ImGui::GetTextLineHeight());
                    ImGui::Text("Indices : %d, %lu", io.MetricsRenderIndices, performanceParams.indexCount);

                    ImGui::SameLine(0, ImGui::GetTextLineHeight());
                    ImGui::Text("Triangles : %d, %lu", io.MetricsRenderIndices / 3, performanceParams.indexCount / 3);

                    ImGui::SameLine(0, ImGui::GetTextLineHeight());
                    ImGui::Text("DrawCall : %lu", performanceParams.drawCallCount);
                }
                AdApplication::GetAppContext()->renderCxt->ResetPerformanceParams();
                ImGui::EndChild();
                ImGui::PopStyleColor();
            }
            ImGui::End();*/
            ImGui::PopStyleVar();

            if(bOpens.bOpenDemoWindow){
                ImGui::ShowDemoWindow(&bOpens.bOpenDemoWindow);
                //ImPlot::ShowDemoWindow(&bOpens.bOpenDemoWindow);
            }

            /*if(bOpens.bOpenContentWindow){
                mEditorContentWindow.Draw(&bOpens.bOpenContentWindow);
            }
            */

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
            std::string viewportName = "Viewport" + std::to_string(mViewportWindows.size());
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

    void DrawViewportWindow(AdRenderTarget *renderTarget, float deltaTime = 0.0f){
        /*if(mViewportWindows.find(renderTarget) != mViewportWindows.end()){
            mViewportWindows[renderTarget]->Draw(nullptr, mViewportDockId);
        }*/
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
                    AdScene *scene = AdApplication::GetAppContext()->scene;
                    if(scene){
                        scene->GetEcsRegistry().view<AdLookAtCameraComponent>().each([scene](const auto &e, AdLookAtCameraComponent &cameraComp){
                            AdEntity *entity = scene->GetEntity(e);
                            if(ImGui::MenuItem(entity->GetName().c_str(), nullptr, false)){
                                LOG_T("{0}", entity->GetName());
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
     void AdEditorMainWindow::DrawViewportWindow(AdRenderTarget *renderTarget, float deltaTime) {
        if(mViewportWindows.find(renderTarget) != mViewportWindows.end()){
            mViewportWindows[renderTarget]->Draw(nullptr, mViewportDockId);
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