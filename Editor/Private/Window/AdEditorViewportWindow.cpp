#include "Window/AdEditorViewportWindow.h"
#include "AdFileUtil.h"
#include "AdApplication.h"
#include "Render/AdRenderContext.h"
#include "Render/AdTexture.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKFrameBuffer.h"
#include "Graphic/AdVKImageView.h"
#include "ECS/Component/AdTransformComponent.h"
#include "ECS/Component/AdCameraComponent.h"
#include "Gui/AdFontAwesomeIcons.h"
#include "Gui/Widget/AdWidget.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_internal.h"

namespace ade{
    AdEditorViewportWindow::AdEditorViewportWindow(AdRenderTarget *renderTarget, const std::string &name) : mRenderTarget(renderTarget), mName(name) {
        CALL_VK(AdApplication::GetContext()->renderCT->GetDevice()->CreateSampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, &mSampler));
    }

    AdEditorViewportWindow::~AdEditorViewportWindow() {
        VK_D(Sampler, AdApplication::GetContext()->renderCT->GetDevice()->GetHandle(), mSampler);
    }

    void AdEditorViewportWindow::Draw(bool *pOpen, ImGuiID dockId, float deltaTime) {
        if(!mRenderTarget){
            return;
        }
        FrameInfo frameInfo = mRenderTarget->GetFrameInfo();
        if(frameInfo.acquireBufferIndex > mDescriptorSets.size() - 1){
            return;
        }
        //const char *title = mName.empty() ? "Unknown" : mName.c_str();
        //char buf[256];
        //sprintf(buf, "Viewport %s %s ###NAME", "-", title);

        std::string title = ade::icon::Camera;
        title += " " + mName;
        ImGui::SetNextWindowDockID(dockId, ImGuiCond_FirstUseEver);
        if(ImGui::Begin(title.c_str(), pOpen, ImGuiWindowFlags_UnsavedDocument)){
            ImGui::GetWindowDrawList()->_OwnerName = title.c_str(); // !!! important !!! Without this, ImGuiZmo will cannot be selected and used !

            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 imageSize = ImGui::GetContentRegionAvail();

            ProcessInput(deltaTime);

            VkExtent2D newExtent = { static_cast<uint32_t>(imageSize.x), static_cast<uint32_t>(imageSize.y) };
            if(!ImGui::IsMouseDown(ImGuiMouseButton_Left)){
                if(newExtent.width != mRenderTarget->GetExtent().width || newExtent.height != mRenderTarget->GetExtent().height){
                    RecreateRenderTarget(newExtent);
                }
            }

            ImVec2 imagePos = ImGui::GetCursorScreenPos();
            if(mDescriptorSets[frameInfo.acquireBufferIndex] != VK_NULL_HANDLE){
                ImGui::Image(mDescriptorSets[frameInfo.acquireBufferIndex], imageSize, { 0, 0 }, { 1, 1});
            }

            AppContext *appContext = AdApplication::GetContext();
            AdNode *pickedNode = appContext->scene->GetPickerNode();
            if(pickedNode && pickedNode->IsValid()){
                auto &transformComp = pickedNode->GetComponent<AdTransformComponent>();
                AdNode *camera = mRenderTarget->GetCamera();
                if(camera && camera->IsValid()){
                    auto &cameraComp = camera->GetComponent<AdPerspCameraComponent>();
                    mInput.bTransformWidgetIsUsing = mTransformWidget.Draw(windowPos, imageSize, transformComp, false, cameraComp.GetProjMat(), cameraComp.GetViewMat());
                } else {
                    mInput.bTransformWidgetIsUsing = mTransformWidget.Draw(windowPos, imageSize, transformComp);
                }
            }

            if(windowSize.x >= 96 && windowSize.y >= 96){
                ShowToolBars({ imagePos.x, imagePos.y + ImGui::GetTextLineHeight() });
            }
        }
        ImGui::End();
    }

    void AdEditorViewportWindow::RecreateRenderTarget(VkExtent2D newExtent) {
        LOG_T("{0} : {1} x {2}", __FUNCTION__, newExtent.width, newExtent.height);
        mRenderTarget->ReCreate(newExtent);

        for (const auto &item: mDescriptorSets){
            if(item != VK_NULL_HANDLE){
                ImGui_ImplVulkan_RemoveTexture(item);
            }
        }
        mDescriptorSets.clear();

        uint32_t bufferCount = mRenderTarget->GetBufferCount();
        for(int i = 0; i < bufferCount; i++){
            AdVKImageView *imageView = mRenderTarget->GetFrameBuffer(i)->GetFinalColorAttachment();
            mDescriptorSets.push_back(ImGui_ImplVulkan_AddTexture(mSampler, imageView->GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        }
    }

    void AdEditorViewportWindow::ProcessInput(float deltaTime) {
        ImGuiIO &io = ImGui::GetIO();
        // camera rotation
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
            mInput.bStartMouseDrag = false;
        }
        AdNode *camera = mRenderTarget->GetCamera();
        if(!camera || !camera->IsValid() || !camera->HasComponent<AdTransformComponent>() || !camera->HasComponent<AdPerspCameraComponent>()){
            return;
        }
        auto &cameraTransComp = camera->GetComponent<AdTransformComponent>();
        auto &cameraComp = camera->GetComponent<AdPerspCameraComponent>();
        if(ImGui::IsWindowHovered()){
            if(ImGui::IsMouseDown(ImGuiMouseButton_Left)){
                mInput.bStartMouseDrag = true;
            }
            if(abs(io.MouseWheel) > 0.1f){
                cameraComp.SetRadius(cameraComp.GetRadius() + io.MouseWheel * -2.f);
            }
        }

        ImVec2 mousePos = ImGui::GetMousePos();
        if(!mInput.bTransformWidgetIsUsing && mInput.bStartMouseDrag){
            ImVec2 mouseDelta = { mousePos.x - mInput.mousePose.x, mousePos.y - mInput.mousePose.y };
            if(abs(mouseDelta.x) > 0.1f || abs(mouseDelta.y) > 0.1f){
                if(!mInput.bFirstMouseDrag){
                    glm::vec3 offset = { mouseDelta.y * 0.35f, mouseDelta.x * -0.35f, 0.f };
                    glm::vec3 rot = cameraTransComp.GetRotation() + offset;
                    if(rot.x > 89.0f){
                        rot.x = 89.0f;
                    }
                    if(rot.x < -89.0f){
                        rot.x = -89.0f;
                    }
                    cameraTransComp.SetRotate(rot);
                } else {
                    mInput.mousePose = mousePos;
                    mInput.bFirstMouseDrag = false;
                }
            }
        }
        mInput.mousePose = mousePos;

        // focus
        if(ImGui::IsWindowHovered()){
            if(ImGui::IsKeyPressed(ImGuiKey_F)){
                AppContext *appContext = AdApplication::GetContext();
                AdNode *pickedNode = appContext->scene->GetPickerNode();
                if(pickedNode && pickedNode->IsValid() && camera->HasComponent<AdPerspCameraComponent>()){
                    glm::vec3 position = pickedNode->GetComponent<AdTransformComponent>().GetPosition();
                    cameraComp.SetTarget(position);
                }
            }
        }

        if(ImGui::IsWindowFocused()){
            if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
                ImGuizmo::OPERATION operation = mTransformWidget.GetOperation();
                int coordMode = mTransformWidget.GetCoordinateMode();
                if(ImGui::IsKeyPressed(ImGuiKey_Q)){
                    mTransformWidget.SetCoordinateMode(static_cast<ImGuizmo::MODE>((coordMode + 1) % 2));
                } else if(ImGui::IsKeyPressed(ImGuiKey_W) && operation != ImGuizmo::TRANSLATE){
                    mTransformWidget.SetOperation(ImGuizmo::TRANSLATE);
                } else if(ImGui::IsKeyPressed(ImGuiKey_E) && operation != ImGuizmo::ROTATE){
                    mTransformWidget.SetOperation(ImGuizmo::ROTATE);
                } else if(ImGui::IsKeyPressed(ImGuiKey_R) && operation != ImGuizmo::SCALE){
                    mTransformWidget.SetOperation(ImGuizmo::SCALE);
                }
            }
        }
    }

    void AdEditorViewportWindow::ShowToolBars(ImVec2 workPos) {
        AdApplication *app = AdApplication::GetContext()->app;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.16f, 0.16f, 0.45f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
                                        | ImGuiWindowFlags_NoDocking
                                        | ImGuiWindowFlags_NoSavedSettings
                                        | ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(workPos);
        float size = 26.f;
        uint32_t itemCount = 3;
        if(ImGui::BeginChild("##toolbar_1", ImVec2(0, size), ImGuiChildFlags_None, window_flags)){
            ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * itemCount * 0.5f));
            ImGui::BeginGroup();

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

            if(ImGui::Button(true ? ade::icon::VectorSquare : ade::icon::DiceD6, ImVec2(size, size))){

            } ImGui::SameLine();
            if(ImGui::Button(ade::icon::Camera, ImVec2(size, size))){
                AdTexture::SaveImageToDisk(mRenderTarget->GetFrameBuffer(0)->GetFinalColorImage(), AD_RES_TEXTURE_DIR"screenshot.png");
            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();

        ImGui::SetNextWindowPos({ workPos.x + ImGui::GetTextLineHeight(), workPos.y });
        if(ImGui::BeginChild("##toolbar_2", ImVec2(size, 0), ImGuiChildFlags_None, window_flags)){
            ImGui::BeginGroup();

            bool pushColor = false;
            if(mTransformWidget.GetOperation() == ImGuizmo::TRANSLATE){
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.66f, 0.f, 1.f));
                pushColor = true;
            }
            if(ImGui::Button(ade::icon::ArrowsAlt, ImVec2(size, size))){
                mTransformWidget.SetOperation(ImGuizmo::TRANSLATE);
            }
            if(pushColor){
                ImGui::PopStyleColor();
                pushColor = false;
            }

            if(mTransformWidget.GetOperation() == ImGuizmo::ROTATE){
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.66f, 0.f, 1.f));
                pushColor = true;
            }
            if(ImGui::Button(ade::icon::SyncAlt, ImVec2(size, size))){
                mTransformWidget.SetOperation(ImGuizmo::ROTATE);
            }
            if(pushColor){
                ImGui::PopStyleColor();
                pushColor = false;
            }

            if(mTransformWidget.GetOperation() == ImGuizmo::SCALE){
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.66f, 0.f, 1.f));
                pushColor = true;
            }
            if(ImGui::Button(ade::icon::CompressAlt, ImVec2(size, size))){
                mTransformWidget.SetOperation(ImGuizmo::SCALE);
            }
            if(pushColor){
                ImGui::PopStyleColor();
            }

            ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.95f, 0.95f, 0.95f, 1.f));
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.f);
            ImGui::PopStyleColor();
            int coordMode = mTransformWidget.GetCoordinateMode();
            if(ImGui::Button(coordMode == ImGuizmo::WORLD ? ade::icon::GlobeAmericas : ade::icon::Cube, ImVec2(size, size))){
                mTransformWidget.SetCoordinateMode(static_cast<ImGuizmo::MODE>((coordMode + 1) % 2));
            }

            if(ImGui::Button(ade::icon::Video, ImVec2(size, size))){
                ImGui::OpenPopup("camera_speed_popup");
            }
            ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.65f, 0.65f, 0.65f, 0.45f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.16f, 0.16f, 0.45f));
            if (ImGui::BeginPopup("camera_speed_popup")){
                AdNode *camera = mRenderTarget->GetCamera();
                if(!camera || !camera->IsValid() || !camera->HasComponent<AdTransformComponent>() || !camera->HasComponent<AdPerspCameraComponent>()){
                    return;
                }
                auto &cameraComp = camera->GetComponent<AdPerspCameraComponent>();
                ImGui::DragFloat("##Speed", &cameraComp.mCameraSpeed, 5.f, 0.f, 99999.f, "%.1f");
                ImGui::EndPopup();
            }
            ImGui::PopStyleColor(2);

            ImGui::EndGroup();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }
}