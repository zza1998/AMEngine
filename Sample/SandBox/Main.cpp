#include <AdEditorApp.h>
#include <ECS/System/AdParticleInitSystem.h>

#include <Loader/ModelLoader.h>

#include "AdEntryPoint.h"
#include "AdFileUtil.h"
#include "Render/AdRenderTarget.h"
#include "Render/AdMesh.h"
#include "Render/AdRenderer.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKCommandBuffer.h"

#include "ECS/AdEntity.h"
#include "ECS/System/AdBaseMaterialSystem.h"
#include "ECS/Component/AdLookAtCameraComponent.h"
#include "ECS/System/AdPhongMaterialSystem.h"
#include "ECS/System/AdPBRMaterialSystem.h"

#include "Event/AdEventObserver.h"

class GalaxyRenderApp : public ade::AdEditorApp {
protected:
    void OnConfiguration(ade::AppSettings *appSettings) override {
        appSettings->width = 1360;
        appSettings->height = 768;
        appSettings->title = "Galaxy";
    }


    void buildGbufferPass() {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        std::vector<ade::Attachment> attachments = {
            //
            {
                .format = swapchain->GetSurfaceInfo().surfaceFormat.format,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
            }
        };
        std::vector<ade::RenderSubPass> subpasses = {
            {
                .colorAttachments = {0},
                .sampleCount = VK_SAMPLE_COUNT_1_BIT
            },
        };
        mRenderPass = std::make_shared<ade::AdVKRenderPass>(
            device, attachments, subpasses);


        mRenderTarget = std::make_shared<ade::AdRenderTarget>(mRenderPass.get());
        mRenderTarget->SetColorClearValue({0.1f, 0.2f, 0.3f, 1.f});
        mRenderTarget->SetDepthStencilClearValue({1, 0});
        // mRenderTarget->AddGBufferRenderSystem();
        // mRenderTarget->AddLightRenderSystem();
    }

    void OnInit() override {
        // init imgui
        AdEditorApp::OnInit();
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        buildGbufferPass();
        mParticleInitSystem = std::make_shared< ade::AdParticleInitSystem>();
        mParticleInitSystem->initParticles();
        mRenderer = std::make_shared<ade::AdRenderer>();

        mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffer(swapchain->GetImages().size());

        std::vector<ade::AdVertex> vertices;
        std::vector<uint32_t> indices;
        ade::AdGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
    }

    void OnSceneInit(ade::AdScene *scene) override {
        // Camera
        ade::AdEntity *camera = scene->CreateEntity("Editor Camera");
        auto &cameraComp = camera->AddComponent<ade::AdLookAtCameraComponent>();
        cameraComp.SetRadius(2.f);
        mRenderTarget->SetCamera(camera);
    }

    void OnSceneDestroy(ade::AdScene *scene) override {
    }

    void OnUpdate(float deltaTime) override {
        ade::AdEntity *camera = mRenderTarget->GetCamera();
        if (ade::AdEntity::HasComponent2<ade::AdLookAtCameraComponent>(camera)) {
            auto &cameraComp = camera->GetComponent<ade::AdLookAtCameraComponent>();
            cameraComp.UpdateCamera(deltaTime);
        }
    }

    void OnRender() override {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();


        if (mRenderer->Begin(&imageIndex)) {
            mRenderTarget->SetExtent({swapchain->GetWidth(), swapchain->GetHeight()});
            mGuiRenderTarget->SetExtent({swapchain->GetWidth(), swapchain->GetHeight()});
        }

        VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
        ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffer);
        // render to gBuffer
        mRenderTarget->Begin(cmdBuffer);
        // render to screen
        //vkCmdNextSubpass(cmdBuffer, VK_SUBPASS_CONTENTS_INLINE);
        mRenderTarget->RenderSkyBox(cmdBuffer);
        // transparent object

        mRenderTarget->End(cmdBuffer);
        // postprocess

        ade::AdVKCommandPool::EndCommandBuffer(cmdBuffer);

        AdEditorApp::OnRender();

        //  swapchain end
        if (mRenderer->End(imageIndex, {cmdBuffer, mGuiCmdBuffers[imageIndex]})) {
            mRenderTarget->SetExtent({swapchain->GetWidth(), swapchain->GetHeight()});
            mGuiRenderTarget->SetExtent({swapchain->GetWidth(), swapchain->GetHeight()});
        }
    }


    void OnDestroy() override {
        ade::AdRenderContext *renderCxt = ade::AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        vkDeviceWaitIdle(device->GetHandle());
        mCmdBuffers.clear();
        mRenderPass.reset();
        mRenderer.reset();
        mRenderTarget.reset();
        mObserver.reset();
        AdEditorApp::OnDestroy();
    }

private:
    std::shared_ptr<ade::AdVKRenderPass> mRenderPass;
    std::shared_ptr<ade::AdRenderTarget> mRenderTarget;
    std::shared_ptr<ade::AdRenderer> mRenderer;
    std::vector<VkCommandBuffer> mCmdBuffers;
    std::shared_ptr<ade::AdParticleInitSystem> mParticleInitSystem;

};

ade::AdApplication *CreateApplicationEntryPoint() {
    return new GalaxyRenderApp();
}
