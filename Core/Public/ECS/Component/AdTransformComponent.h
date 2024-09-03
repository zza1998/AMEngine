#ifndef AD_TRANSFORMCOMPONENT_H
#define AD_TRANSFORMCOMPONENT_H

#include "ECS/AdComponent.h"
#include "AdGraphicContext.h"

namespace ade{
    class AdTransformComponent : public AdComponent {
    public:
        glm::vec3 position{ 0.f, 0.f, 0.f };
        glm::vec3 rotation{ 0.f, 0.f, 0.f };  // degree
        glm::vec3 scale{ 1.f, 1.f, 1.f };

        glm::mat4 GetTransform() const{
            glm::mat4 transMat = glm::translate(glm::mat4(1.f), position);
            glm::mat4 rotationMat = glm::rotate(glm::mat4(1.f), glm::radians(rotation.x), glm::vec3{ 1, 0, 0 });
            rotationMat = glm::rotate(rotationMat, glm::radians(rotation.y), glm::vec3{ 0, 1, 0 });
            rotationMat = glm::rotate(rotationMat, glm::radians(rotation.z), glm::vec3{ 0, 0, 1 });
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), scale);
            return transMat * rotationMat * scaleMat;
        }
    };
}

#endif
