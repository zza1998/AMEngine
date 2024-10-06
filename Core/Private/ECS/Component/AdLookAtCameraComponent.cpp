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
        if(AdEntity::HasComponent2<AdTransformComponent>(owner)){
            auto &transComp = owner->GetComponent<AdTransformComponent>();
            float yaw = transComp.GetRotation().x;
            float pitch = transComp.GetRotation().y;

            glm::vec3 direction;
            direction.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            direction.y = sin(glm::radians(pitch));
            direction.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
            mTarget = transComp.GetPosition() + direction * mRadius;
            //transComp.SetPosition(mTarget);

            mViewMat = glm::lookAt(transComp.GetPosition(), mTarget, mWorldUp);
        }
        return mViewMat;
    }

    void AdLookAtCameraComponent::SetViewMat(const glm::mat4 &viewMat) {
        // TODO
    }

    const glm::vec3 & AdLookAtCameraComponent::GetCamPosition() {
        AdEntity *owner = GetOwner();
        if(AdEntity::HasComponent2<AdTransformComponent>(owner)) {
            AdTransformComponent &transComp = owner->GetComponent<AdTransformComponent>();
            mPosition = transComp.GetPosition();
        }
        return mPosition;
    }

    void AdLookAtCameraComponent::UpdateCamera(float deltaTime) const {
        AdEntity *owner = GetOwner();
        if(AdEntity::HasComponent2<AdTransformComponent>(owner)) {
            AdTransformComponent &transComp = owner->GetComponent<AdTransformComponent>();
            float change = 10 * deltaTime;

            if(mForwardFlag) {
                transComp.SetPosition(transComp.GetPosition()  + change * glm::vec3(0, 0, 1));
            }
            if(mBackwardFlag) {
                transComp.SetPosition(transComp.GetPosition()  + change * glm::vec3(0, 0, -1));
            }
            if(mLeftFlag) {
                transComp.SetPosition(transComp.GetPosition()  + change * glm::vec3(1, 0, 0));
            }
            if(mRightFlag) {
                transComp.SetPosition(transComp.GetPosition()  + change * glm::vec3(-1, 0, 0));
            }
            if(mUpFlag) {
                transComp.SetPosition(transComp.GetPosition()  + change * glm::vec3(0, -1, 0));
            }
            if(mDownFlag) {
                transComp.SetPosition(transComp.GetPosition()  + change * glm::vec3(0, 1, 0));
            }
        }


    }
}
