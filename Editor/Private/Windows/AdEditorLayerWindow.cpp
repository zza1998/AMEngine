#include "Windows/AdEditorLayerWindow.h"
#include "AdApplication.h"
#include "ECS/AdEntity.h"
#include "Gui/AdFontAwesomeIcons.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "AdLog.h"

namespace ade{
    void AdEditorLayerWindow::Draw(bool *pOpen) {
        AdScene *scene = AdApplication::GetAppContext()->scene;
        if (ImGui::Begin("Layout", pOpen)){
            if(scene){
                static ImGuiTableFlags flags = ImGuiTableFlags_None;
                ImGui::SetNextItemWidth(FLT_MAX);
                if(ImGui::BeginTable("Node Table", 2, flags)){
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None);
                    ImGui::TableSetupColumn("Buttons", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x * 0.3f);
                    AdNode *rootNode = scene->GetRootNode();
                    if(rootNode->HasChildren()){
                        std::vector<AdNode*> nodes = rootNode->GetChildren();
                        for (const auto &node: nodes){
                            if(node){
                                AdNode *pickedNode = scene->GetPickedNode();
                                DrawNodeTree(node, &pickedNode);
                                if(pickedNode != scene->GetPickedNode()){
                                    LOG_T("Select node: {0}", pickedNode->GetId());
                                    scene->SetPickedNode(pickedNode);
                                }
                            }
                        }
                    }
                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();
    }

    void AdEditorLayerWindow::DrawNodeTree(AdNode *node, AdNode **pickedNode) {
        if(!node) return;
        ImGui::TableNextRow();
        // buttons
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::PushID(node);
//        DrawNodeTreeButtons(node);
        ImGui::PopID();

        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAllColumns;
        if(!node->HasChildren()){
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
        }
        if(node == *pickedNode){
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        std::string nodeLabel = std::string(ade::icon::DiceD6) + " " + node->GetName();
        bool open = ImGui::TreeNodeEx(nodeLabel.c_str(), flags);
        if(open){
            if(ImGui::IsItemClicked()){
                *pickedNode = node;
            }
            if(node->HasChildren()){
                std::vector<AdNode*> nodes = node->GetChildren();
                for (const auto &child: nodes){
                    if(child){
                        DrawNodeTree(child, pickedNode);
                    }
                }
            }
            ImGui::TreePop();
        } else {
            // open or close both to be selected
            if(ImGui::IsItemClicked()){
                *pickedNode = node;
            }
        }
    }
//
//    void AdEditorLayerWindow::DrawNodeTreeButtons(AdNode* node) {
//        AdScene *scene = AdApplication::GetContext()->scene;
//        auto &nodeStatusComp = node->GetComponent<AdNodeStatusComponent>();
//
//        ImGui::PushStyleColor(ImGuiCol_Button, ImGuiCol_WindowBg);
//        ImGui::BeginGroup();
//        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
//        float size = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
//
//        ImGui::BeginGroup();
//        if (ImGui::Button(nodeStatusComp.bHidden ? ade::icon::EyeSlash : ade::icon::Eye, ImVec2{ size, size })){
//            nodeStatusComp.bHidden = !nodeStatusComp.bHidden;
//        }
//        ImGui::PopItemWidth();
//        ImGui::SameLine(size + 1.f);
//
//        if(ImGui::Button(nodeStatusComp.bCanPick ? ade::icon::LocationArrow : ade::icon::Ban, ImVec2{ size, size })){
//            nodeStatusComp.bCanPick = !nodeStatusComp.bCanPick;
//        }
//        ImGui::PopItemWidth();
//        ImGui::SameLine(size * 2 + 1.f);
//        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImVec4(0.66f, 0.f, 0.f, 1.f)));
//        if (ImGui::Button(ade::icon::Times, ImVec2{ size, size })){
//            ImGui::OpenPopup("Delete Node Popup");
//        }
//        ImGui::PopItemWidth();
//        ImGui::EndGroup();
//        ImGui::PopStyleColor(2);
//
//        if (ImGui::BeginPopup("Delete Node Popup")){
//            if(ImGui::Button("Delete this Node?")){
//                scene->DestroyNode(node);
//                ImGui::CloseCurrentPopup();
//            }
//            ImGui::EndPopup();
//        }
//        ImGui::EndGroup();
//    }
}