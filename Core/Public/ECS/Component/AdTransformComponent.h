#ifndef AD_TRANSFORMCOMPONENT_H
#define AD_TRANSFORMCOMPONENT_H

#include <AdLog.h>

#include "ECS/AdComponent.h"
#include "AdGraphicContext.h"
#include "glm/gtx/matrix_decompose.hpp"

namespace ade{
    class AdTransformComponent : public AdComponent {
    public:

        void UpdateTransform();
        glm::mat4 GetTransform ();
        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotation);
        void SetScale(glm::vec3 scale);
        void SetTransform(const glm::mat4 &matrix);

        [[nodiscard]] glm::vec3 GetPosition() const {
            return mPosition;
        }

        [[nodiscard]] glm::vec3 GetRotation() const {
            return mRotationEuler;
        }

        [[nodiscard]] glm::vec3 GetScale() const {
            return mScale;
        }

        void OnDrawGui() override;
        REG_COMPONENT(AdTransformComponent);
    private:
        glm::vec3 mPosition{ 0.f, 0.f, 0.f };
        glm::qua<float> mRotationQua{ 0.f, 0.f, 0.f ,0.f};  // degree
        glm::vec3 mRotationEuler{ 0.f, 0.f, 0.f};  // degree
        glm::vec3 mScale{ 1.f, 1.f, 1.f };
        glm::mat4 mTransform = glm::mat4{1.0f};
        bool updated = false;
    };

}

#endif
