#include "ECS/System/AdMaterialSystem.h"

#include "AdApplication.h"
#include "Render/AdRenderContext.h"
#include "Render/AdRenderTarget.h"
#include "ECS/Component/AdLookAtCameraComponent.h"

namespace ade{
    AdApplication *AdMaterialSystem::GetApp() const {
        AdAppContext *appContext = AdApplication::GetAppContext();
        if(appContext){
            return appContext->app;
        }
        return nullptr;
    }

    AdScene *AdMaterialSystem::GetScene() const {
        AdAppContext *appContext = AdApplication::GetAppContext();
        if(appContext){
            return appContext->scene;
        }
        return nullptr;
    }

    AdVKDevice *AdMaterialSystem::GetDevice() const {
        AdAppContext *appContext = AdApplication::GetAppContext();
        if(appContext){
            if(appContext->renderCxt){
                return appContext->renderCxt->GetDevice();
            }
        }
        return nullptr;
    }

    const glm::mat4 AdMaterialSystem::GetProjMat(AdRenderTarget *renderTarget) const {
        glm::mat4 projMat{1.f};
        AdEntity *camera = renderTarget->GetCamera();
        if(AdEntity::HasComponent2<AdLookAtCameraComponent>(camera)){
            auto &cameraComp = camera->GetComponent<AdLookAtCameraComponent>();
            projMat = cameraComp.GetProjMat();
        }
        return projMat;
    }

    const glm::mat4 AdMaterialSystem::GetViewMat(AdRenderTarget *renderTarget) const {
        glm::mat4 viewMat{1.f};
        AdEntity *camera = renderTarget->GetCamera();
        if(AdEntity::HasComponent2<AdLookAtCameraComponent>(camera)){
            auto &cameraComp = camera->GetComponent<AdLookAtCameraComponent>();
            viewMat = cameraComp.GetViewMat();
        }
        return viewMat;
    }
}