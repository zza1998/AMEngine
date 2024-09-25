#include "Gui/Widget/AdWidget.h"
#include "imgui/imgui_internal.h"
#include "glm/gtc/quaternion.hpp"

namespace ade{
    ImVec4 colors[3] = {
        ImVec4(0.66f, 0.f, 0.f, 1.f),
        ImVec4(0.f, 0.66f, 0.f, 1.f),
        ImVec4(0.f, 0.f, 0.66f, 1.f)
    };

    bool AdWidget::BeginGuiField(const char *fieldName, ImGuiTextFilter *filter){
        if(filter){
            if(!filter->PassFilter(fieldName)){
                return false;
            }
        }
        ImGui::PushID(fieldName);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", fieldName);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
        return true;
    }

    void AdWidget::EndGuiField(){
        ImGui::PopID();
    }

    bool AdWidget::DrawOneFloat(const char *fieldName, float *v,float v_speed , float v_min , float v_max ) {
        BeginGuiField(fieldName);
        bool value_changed = false;
        value_changed |= ImGui::DragFloat(fieldName,v,v_speed,v_min,v_max);
        EndGuiField();
        return value_changed;
    }
    bool AdWidget::DrawVecColorField(const char *fieldName, glm::vec4 &v, float v_speed, float v_min, float v_max) {

        ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoDragDrop ;
        BeginGuiField(fieldName);
        ImGui::ColorEdit4("MyColor##2f", (float*)&v.x, ImGuiColorEditFlags_Float | misc_flags);
        EndGuiField();
        return true;
    }

    static void DrawFloatDragColorReact(ImU32 color){
        float sz = ImGui::GetFrameHeight();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz * 0.3f, p.y + sz), color);
        ImGui::Dummy(ImVec2(sz * 0.3f, sz));
    }

    bool AdWidget::DrawVec3Field(const char *fieldName, float *v, float v_speed, float v_min, float v_max, const char *format) {
        AdWidget::BeginGuiField(fieldName);

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems) {
            return false;
        }

        ImGuiContext& g = *GImGui;
        bool value_changed = false;
        ImGui::BeginGroup();
        ImGui::PushID(fieldName);
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        size_t type_size = sizeof(float);
        for (int i = 0; i < 3; i++)
        {
            ImGui::PushID(i);
            if (i > 0)
                ImGui::SameLine(0, 0.1f);

            if(i <= ARRAY_SIZE(colors) - 1){
                DrawFloatDragColorReact(ImGui::GetColorU32(colors[i]));
            } else {
                DrawFloatDragColorReact(ImGui::GetColorU32((ImGuiCol)22+i));
            }
            ImGui::SameLine(0, 0);

            value_changed |= ImGui::DragScalar("", ImGuiDataType_Float, v, v_speed, &v_min, &v_max, format, 0);
            ImGui::PopID();
            ImGui::PopItemWidth();
            v = (float*)((char*)v + type_size);
        }
        ImGui::PopID();

        const char* label_end = ImGui::FindRenderedTextEnd(fieldName);
        if (fieldName != label_end)
        {
            ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
            ImGui::TextEx(fieldName, label_end);
        }

        ImGui::EndGroup();
        AdWidget::EndGuiField();
        return value_changed;
    }

    bool AdWidget::DrawRotation3Field(const char *fieldName, glm::vec3 &v, float v_speed, float v_min, float v_max) {
        static int dataType = 0;
        bool changed = false;
        switch (dataType) {
            case 0:
            {
                changed = AdWidget::DrawVec3Field(fieldName, &v.x, v_speed, v_min, v_max, "%.2f °");
            }
                break;
            case 1:
            {
                AdWidget::BeginGuiField(fieldName);
                glm::vec3 rad = glm::radians(v);
                changed = AdWidget::DrawVec3Field("##value", &rad.x, v_speed, v_min, v_max, "%.3f");
                if(changed){
                    v = glm::degrees(rad);
                }
                AdWidget::EndGuiField();
            }
                break;
        }

        if (ImGui::BeginPopupContextItem("Rotation PopupContext")){
            if(ImGui::RadioButton("Degree", dataType == 0)){
                dataType = 0;
                ImGui::CloseCurrentPopup();
            }

            if(ImGui::RadioButton("Radian", dataType == 1)){
                dataType = 1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return changed;
    }

    bool AdWidget::DrawRotation3Field(const char *fieldName, glm::qua<float> &v, float v_speed, float v_min, float v_max) {
        static int dataType = 0;
        bool changed = false;
        switch (dataType) {
            case 0:
            {
                glm::vec3 euler = glm::degrees(glm::eulerAngles(v));
                changed = AdWidget::DrawVec3Field(fieldName, &euler.x, v_speed, v_min, v_max, "%.2f °");
            }
                break;
            case 1:
            {
                AdWidget::BeginGuiField(fieldName);
                changed = ImGui::DragFloat4("##value", &v.x, v_speed, v_min, v_max, "%.3f");
                AdWidget::EndGuiField();
            }
                break;
        }

        if (ImGui::BeginPopupContextItem("Rotation PopupContext")){
            if(ImGui::RadioButton("Euler", dataType == 0)){
                dataType = 0;
                ImGui::CloseCurrentPopup();
            }

            if(ImGui::RadioButton("Quaternion", dataType == 1)){
                dataType = 1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return changed;
    }

    void AdWidget::DefinePopupInfo(const char *title, const std::function<void(void)> &funcUIDefine) {
        if (ImGui::BeginPopup(title,ImGuiChildFlags_AutoResizeY)){
            funcUIDefine();
            ImGui::EndPopup();
        }
    }

    bool AdWidget::DefinePopupConfirm(const char *title, bool *p_open,
                                                    const std::function<void(void)> &funcUIDefine,
                                                    const std::function<void(void)> &funcConfirm,
                                                    const std::function<void(void)> &funcCancel, ImGuiWindowFlags flags) {
        bool isOk = false;
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal(title, p_open, flags)){
            funcUIDefine();

            ImGui::Text("%s", "");
            ImGui::SameLine(0.f, 100.f);
            ImGui::Text("%s", "");
            ImGui::Separator();
            if (ImGui::Button("Confirm", ImVec2(50, 0))){
                funcConfirm();
                isOk = true;
            }

            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(50, 0))) {
                funcCancel();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return isOk;
    }
}