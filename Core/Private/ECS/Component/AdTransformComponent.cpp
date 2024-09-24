//
// Created by zhou on 2024/9/9.
//

#include "ECS/Component/AdTransformComponent.h"

#include <Gui/Widget/AdWidget.h>

namespace ade {
    void AdTransformComponent::SetTransform(const glm::mat4 &matrix) {
        glm::vec3 position;
        glm::fquat rotationQua;
        glm::vec3 scale;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(matrix, scale, rotationQua, position, skew, perspective);

        mPosition = position;
        mRotationQua = rotationQua;
        mScale = scale;
        updated = true;
    }

    glm::mat4 AdTransformComponent::GetTransform()   {
        if(updated) {
            UpdateTransform();
            updated = false;
        }
        return mTransform;
    }

    void AdTransformComponent::SetPosition(glm::vec3 position) {
        mPosition = position;
        updated = true;
    }

    void AdTransformComponent::SetRotation(glm::vec3 rotation) {
        mRotationEuler = rotation;
        mRotationQua = glm::quat(rotation);
        updated = true;
    }

    void AdTransformComponent::SetScale(glm::vec3 scale) {
        mScale = scale;
        updated = true;
    }

    void AdTransformComponent::UpdateTransform() {
        glm::mat4 transMat = glm::translate(glm::mat4(1.f), mPosition);
        glm::mat4 rotationMat = glm::rotate(glm::mat4(1.f), glm::radians(mRotationEuler.x), glm::vec3{ 1, 0, 0 });
        rotationMat = glm::rotate(rotationMat, glm::radians(mRotationEuler.y), glm::vec3{ 0, 1, 0 });
        rotationMat = glm::rotate(rotationMat, glm::radians(mRotationEuler.z), glm::vec3{ 0, 0, 1 });
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), mScale);
        mTransform = transMat * rotationMat * scaleMat;

    }

    void AdTransformComponent::OnDrawGui() {
        if(AdWidget::DrawVec3Field("Position", &mPosition.x, 0.1f)){
            updated = true;
        }
        glm::vec3 rotation = GetRotation();
        if(AdWidget::DrawRotation3Field("Rotation", rotation, 1.f)){
            SetRotation(rotation);
        }
        if(AdWidget::DrawVec3Field("Scale", &mScale.x, 0.1f)){
            updated = true;
        }
    }
}
