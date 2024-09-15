#ifndef AD_LOOKAT_CAMERA_COMPONENT_H
#define AD_LOOKAT_CAMERA_COMPONENT_H

#include "ECS/AdComponent.h"
#include "AdGraphicContext.h"

namespace ade{
    class AdLookAtCameraComponent : public AdComponent{
    public:
        const glm::mat4 &GetProjMat();
        const glm::mat4 &GetViewMat();

        void UpdateCamera(float deltaTime) const;
        void SetViewMat(const glm::mat4 &viewMat);

        float GetFov() const { return mFov; }
        float GetAspect() const { return mAspect; }
        float GetNearPlane() const { return mNearPlane; }
        float GetFarPlane() const { return mFarPlane; }
        float GetRadius() const { return mRadius; }
        const glm::vec3 &GetTarget() const { return mTarget; }

        void SetFov(float fov) { this->mFov = fov; }
        void SetAspect(float aspect) { this->mAspect = aspect; }
        void SetNearPlane(float nearPlane) { this->mNearPlane = nearPlane; }
        void SetFarPlane(float farPlane) { this->mFarPlane = farPlane; }
        void SetRadius(float radius) { this->mRadius = radius; }
        void SetTarget(const glm::vec3 &target) { this->mTarget = target; }

        bool mUpFlag{ false };
        bool mRightFlag{ false };
        bool mDownFlag{ false };
        bool mLeftFlag{ false };
        bool mForwardFlag{ false };
        bool mBackwardFlag{ false };

    private:
        float mFov{ 65.f };
        float mAspect{ 1.f };
        float mNearPlane{ 0.3f };
        float mFarPlane{ 1000.f };
        glm::vec3 mTarget{ 0.f, 0.f, 0.f };
        glm::vec3 mWorldUp{ 0.f, 1.f, 0.f };

        float mRadius{ 6.f };

        glm::mat4 mProjMat{ 1.f };
        glm::mat4 mViewMat{ 1.f };

    };
}

#endif