#include "Window/AdEditorPropertyWindow.h"
#include "AdApplication.h"
#include "ECS/AdNode.h"
#include "ECS/Component/AdNodeStatusComponent.h"

#include "Gui/Widget/AdWidget.h"
#include "imgui/imgui.h"

namespace ade{
    static std::string GetComponentLabel(entt::meta_type &meta){
        std::string componentLabel = std::string(meta.info().name());
        auto labelProp = meta.prop(HS("Label"));
        if(labelProp){
            componentLabel = labelProp.value().cast<const char*>();
        }
        return componentLabel;
    }

    static bool ResolveComponent(AdNode *node, entt::meta_type &meta, entt::meta_any &outInstance){
        auto hasCompFunc = meta.func(HS(FUNC_NAME_HAS_COMPONENT));
        if(!hasCompFunc){
            return false;
        }

        bool hasComp = hasCompFunc.invoke({}, *node).cast<bool>();
        if(!hasComp){
            return false;
        }

        auto getCompFunc = meta.func(HS(FUNC_NAME_GET_COMPONENT));
        if(!getCompFunc){
            LOG_E("Does not contains AdNode::{0}<{1}>() func.", FUNC_NAME_GET_COMPONENT, GetComponentLabel(meta));
            return false;
        }
        outInstance = getCompFunc.invoke({}, *node);
        return true;
    }

    void AdEditorPropertyWindow::Draw(bool *pOpen) {
        if(ImGui::Begin("Property", pOpen)){
            AppContext *appContext = AdApplication::GetContext();
            AdNode *pickedNode = appContext->scene->GetPickerNode();
            if(!pickedNode || !pickedNode->IsValid()){
                ImGui::End();
                return;
            }

            static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV;
            static uint32_t open_action = -1;
            if(ImGui::BeginTable("Property Table", 2, tableFlags)) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, ImGui::GetContentRegionAvail().x * 0.7f);

                int uuid = pickedNode->GetId().mUUID;
                if (AdWidget::BeginGuiField("UUID")) {
                    ImGui::BeginDisabled();
                    ImGui::Text("%d", uuid);
                    ImGui::EndDisabled();
                    AdWidget::EndGuiField();
                }

                if (AdWidget::BeginGuiField("Name")) {
                    char buffer[256];
                    memset(buffer, 0, sizeof(buffer));
                    strncpy(buffer, pickedNode->GetName().c_str(), sizeof(buffer));
                    if (ImGui::InputText("##value", buffer, sizeof(buffer))) {
                        pickedNode->SetName(buffer);
                    }
                    AdWidget::EndGuiField();
                }
                ImGui::EndTable();
            }

            // foreach all component
            entt::registry &ecsRegistry = appContext->scene->GetEcsRegistry();
            std::vector<entt::id_type> notContainComponents;
            for (const auto &storage: ecsRegistry.storage()){
                auto meta = entt::resolve(storage.first);
                if(!meta){
                    continue;
                }
                entt::meta_any instance;
                if(!ResolveComponent(pickedNode, meta, instance)){
                    notContainComponents.push_back(storage.first);
                    continue;
                }

                std::string componentLabel = GetComponentLabel(meta);
                if (open_action != -1) ImGui::SetNextItemOpen(open_action != 0);
                bool open = ImGui::CollapsingHeader(componentLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
                if(ImGui::BeginPopupContextItem(componentLabel.c_str())){
                    if(ImGui::Button("Delete Component")){
                        auto removeCompFunc = meta.func(HS(FUNC_NAME_REMOVE_COMPONENT));
                        if(removeCompFunc){
                            removeCompFunc.invoke({}, *pickedNode);
                        } else {
                            LOG_E("Does not contains AdNode::{0}<{1}>() func.", FUNC_NAME_REMOVE_COMPONENT, componentLabel);
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                if(open && ImGui::BeginTable("Property Table", 2, tableFlags)) {
                    auto drawImGuiFunc = meta.func(HS(FUNC_NAME_DRAW_GUI));
                    if(drawImGuiFunc){
                        drawImGuiFunc.invoke(instance);
                    } else {
                        LOG_E("Does not contains AdNode::{0}<{1}>() func.", FUNC_NAME_DRAW_GUI, componentLabel);
                    }
                    ImGui::EndTable();
                }
            }

            if(!notContainComponents.empty()){
                ImGui::Separator();
                if (ImGui::Button("Add Component", { -1, 0 })) {
                    ImGui::OpenPopup("AddComponent");
                }
                if (ImGui::BeginPopup("AddComponent")){
                    for (const auto &item: notContainComponents){
                        auto meta = entt::resolve(item);
                        if(!meta){
                            continue;
                        }
                        std::string componentLabel = GetComponentLabel(meta);
                        if(ImGui::Button(componentLabel.c_str())){
                            auto addCompFunc = meta.func(HS(FUNC_NAME_ADD_COMPONENT));
                            if(addCompFunc){
                                addCompFunc.invoke({}, *pickedNode);
                            } else {
                                LOG_E("Does not contains AdNode::{0}<{1}>() func.", FUNC_NAME_ADD_COMPONENT, componentLabel);
                            }
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::End();
    }
}