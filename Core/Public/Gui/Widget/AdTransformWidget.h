#ifndef ADTRANSFORMWIDGET_H
#define ADTRANSFORMWIDGET_H

#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "AdGraphicContext.h"

namespace ade{
    class AdTransformComponent;

    class AdTransformWidget{
    public:
        bool Draw(ImVec2 workPos, ImVec2 workSize, AdTransformComponent &transformComp, bool isOrth = true, const glm::mat4 &projMat = glm::mat4{ 1.f }, const glm::mat4 &viewMat = glm::mat4{ 1.f }) const;

        static void SetOperation(ImGuizmo::OPERATION operation) { sOperation = operation; }
        static void SetCoordinateMode(ImGuizmo::MODE coordinateMode) { sCoordinateMode = coordinateMode; }
        static ImGuizmo::OPERATION GetOperation() { return sOperation; }
        static ImGuizmo::MODE GetCoordinateMode() { return sCoordinateMode; }
    private:
        static ImGuizmo::OPERATION sOperation;
        static ImGuizmo::MODE sCoordinateMode;
    };
}

#endif