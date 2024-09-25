#ifndef ADWIDGET_H
#define ADWIDGET_H

#include "AdEngine.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"

namespace ade{
    class AdWidget{
    public:
        static bool BeginGuiField(const char *fieldName, ImGuiTextFilter *filter = nullptr);
        static void EndGuiField();
        static bool DrawVecColorField(const char *fieldName, glm::vec4 &v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f);
        static bool DrawVec3Field(const char *fieldName, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f");
        static bool DrawRotation3Field(const char *fieldName, glm::vec3 &v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f);
        static bool DrawRotation3Field(const char *fieldName, glm::qua<float> &v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f);
        static void DefinePopupInfo(const char *title, const std::function<void(void)> &funcUIDefine);
        static bool DefinePopupConfirm(const char *title, bool *p_open, const std::function<void(void)> &funcUIDefine, const std::function<void(void)> &funcConfirm, const std::function<void(void)> &funcCancel=[](){}, ImGuiWindowFlags flags = 0);
    };
}
#endif