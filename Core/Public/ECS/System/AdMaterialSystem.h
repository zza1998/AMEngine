#ifndef AD_MATERIAL_SYSTEM_H
#define AD_MATERIAL_SYSTEM_H



#include "AdGraphicContext.h"
#include "Graphic/AdVKCommon.h"
#include "ECS/AdSystem.h"


namespace ade{
    class AdVKRenderPass;
    class AdRenderTarget;
    class AdApplication;
    class AdScene;
    class AdVKDevice;
    class AdVKBuffer;
    class AdGbufferRenderSystem;

    class AdMaterialSystem : public AdSystem{
    public:
        virtual void OnInit(AdVKRenderPass *renderPass) = 0;
        virtual void OnRender(VkCommandBuffer cmdBuffer, AdRenderTarget *renderTarget) = 0;
        virtual void SetGbufferSystem(std::shared_ptr<AdGbufferRenderSystem> gbufferRender){};
        virtual void OnDestroy() = 0;
    protected:
        AdApplication *GetApp() const;
        AdScene *GetScene() const;
        AdVKDevice *GetDevice() const;
        const glm::mat4 GetProjMat(AdRenderTarget *renderTarget) const;
        const glm::mat4 GetViewMat(AdRenderTarget *renderTarget) const;
        const glm::vec3 GetCameraPos(AdRenderTarget *renderTarget) const;
    };
}

#endif