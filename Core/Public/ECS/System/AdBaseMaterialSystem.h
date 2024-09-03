#ifndef ADBASEMATERIALSYSTEM_H
#define ADBASEMATERIALSYSTEM_H

#include "ECS/System/AdMaterialSystem.h"
#include "ECS/Component/AdTransformComponent.h"
#include "ECS/Component/Material/AdBaseMaterialComponent.h"
#include "AdGraphicContext.h"

namespace ade{
    class AdVKPipelineLayout;
    class AdVKPipeline;

    struct PushConstants{
        glm::mat4 matrix{ 1.f };
        uint32_t colorType;
    };

    class AdBaseMaterialSystem : public AdMaterialSystem{
    public:
        void OnInit(AdVKRenderPass *renderPass) override;
        void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) override;
        void OnDestroy() override;
    private:
        std::shared_ptr<AdVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<AdVKPipeline> mPipeline;
    };
}

#endif