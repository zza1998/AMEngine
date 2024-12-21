#ifndef ADLIGHT_H
#define ADLIGHT_H

#include "AdGraphicContext.h"
#include "ECS/AdComponent.h"

namespace ade{
    struct DirectLight{
        alignas(16) glm::vec3 ambient{ 0.2f, 0.2f, 0.2f };
        alignas(16) glm::vec3 diffuse{ 0.5f, 0.5f, 0.5f };
        alignas(16) glm::vec3 specular{ 1.0f, 1.0f, 1.0f };
        alignas(16) glm::vec3 direction{ 0.f, 0.f, 0.f };
    };

    struct PointLight{
        alignas(16) glm::vec3 ambient{ 0.2f, 0.2f, 0.2f };
        alignas(16) glm::vec3 diffuse{ 0.5f, 0.5f, 0.5f };
        alignas(16) glm::vec3 specular{ 1.0f, 1.0f, 1.0f };
        alignas(16) glm::vec3 position{ 0.f, 0.f, 0.f };
        alignas(4) float constant = 1.0f;
        alignas(4) float linear = 0.09f;
        alignas(4) float quadratic = 0.032f;
        alignas(4) float used = 0.0f;
    };

    struct Spotlight{
        alignas(16) glm::vec3 ambient{ 0.2f, 0.2f, 0.2f };
        alignas(16) glm::vec3 diffuse{ 0.5f, 0.5f, 0.5f };
        alignas(16) glm::vec3 specular{ 1.0f, 1.0f, 1.0f };
        alignas(16) glm::vec3 position{ 0.f, 0.f, 0.f };
        alignas(16) glm::vec3 spot_direction{ 0.f, 0.4f, 1.3f };
        alignas(4) float cutOff = 24.f;
        alignas(4) float outerCutOff = 30.f;
        alignas(4) float constant = 1.0f;
        alignas(4) float linear = 0.09f;
        alignas(4) float quadratic = 0.032f;
    };

    struct AdDirectLightComponent : public AdComponent {
        DirectLight params{};
        REG_COMPONENT(AdDirectLightComponent);
        void OnDrawGui() override{};
    };

    struct AdPointLightComponent : public AdComponent {
        PointLight params{};
        REG_COMPONENT(AdPointLightComponent);
        void OnDrawGui() override {

        };
    };

    struct AdSpotlightComponent : public AdComponent {
        Spotlight params{};
        REG_COMPONENT(AdSpotlightComponent)
        void OnDrawGui() override{};

    };
}

#endif