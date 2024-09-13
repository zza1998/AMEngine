#include "Gui/Widget/AdTransformWidget.h"
#include "ECS/Component/AdTransformComponent.h"

namespace ade{
    ImGuizmo::OPERATION AdTransformWidget::sOperation = ImGuizmo::ROTATE;
    ImGuizmo::MODE AdTransformWidget::sCoordinateMode = ImGuizmo::WORLD;

    bool AdTransformWidget::Draw(ImVec2 workPos, ImVec2 workSize, AdTransformComponent &transformComp, bool isOrth, const glm::mat4 &projMat, const glm::mat4 &viewMat) const {
        ImGuizmo::SetOrthographic(isOrth);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(workPos.x, workPos.y, workSize.x, workSize.y);

        glm::mat4 modelMatrix = transformComp.GetTransform();

        ImGuizmo::Manipulate(glm::value_ptr(viewMat),
                             glm::value_ptr(projMat),
                             sOperation, sCoordinateMode,
                             glm::value_ptr(modelMatrix),
                             nullptr, nullptr);

        bool bUsing = ImGuizmo::IsUsing();
        if (bUsing){
            transformComp.SetTransform(modelMatrix);
        }
        return bUsing;
    }
}