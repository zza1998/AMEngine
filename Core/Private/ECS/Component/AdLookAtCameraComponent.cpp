#include "ECS/Component/AdLookAtCameraComponent.h"
#include "ECS/Component/AdTransformComponent.h"

namespace ade{
    const glm::mat4 &AdLookAtCameraComponent::GetProjMat() {
        mProjMat = glm::perspective(glm::radians(mFov), mAspect, mNearPlane, mFarPlane);
        mProjMat[1][1] *= -1.f;
        return mProjMat;
    }

    const glm::mat4 &AdLookAtCameraComponent::GetViewMat() {
        AdEntity *owner = GetOwner();
        if(AdEntity::HasComponent<AdTransformComponent>(owner)){
            auto &transComp = owner->GetComponent<AdTransformComponent>();
            float yaw = transComp.rotation.x;
            float pitch = transComp.rotation.y;

            glm::vec3 direction;
            direction.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            direction.y = sin(glm::radians(pitch));
            direction.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));

            transComp.position = mTarget + direction * mRadius;

            mViewMat = glm::lookAt(transComp.position, mTarget, mWorldUp);
        }
        return mViewMat;
    }

    void AdLookAtCameraComponent::SetViewMat(const glm::mat4 &viewMat) {
        // TODO
    }
}