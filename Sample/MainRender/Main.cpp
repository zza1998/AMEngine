#include <AdEditorApp.h>
#include <ECS/Component/Material/AdPBRMaterialComponent.h>
#include <ECS/Component/Material/AdPhongMaterialComponent.h>

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
class MainRenderApp : public ade::AdEditorApp{
protected:
    void OnConfiguration(ade::AppSettings *appSettings) override {
        appSettings->width = 1360;
        appSettings->height = 768;
        appSettings->title = "MainRender";
    }

    void OnInit() override {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        std::vector<ade::Attachment> attachments = {
            {
                .format = swapchain->GetSurfaceInfo().surfaceFormat.format,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
            },
            {
                .format = device->GetSettings().depthFormat,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
            }
        };
        std::vector<ade::RenderSubPass> subpasses = {
            {
                .colorAttachments = { 0 },
                .depthStencilAttachments = { 1 },
                .sampleCount = VK_SAMPLE_COUNT_4_BIT
            }
        };
        mRenderPass = std::make_shared<ade::AdVKRenderPass>(device, attachments, subpasses);

        mRenderTarget = std::make_shared<ade::AdRenderTarget>(mRenderPass.get());
        mRenderTarget->SetColorClearValue({0.1f, 0.2f, 0.3f, 1.f});
        mRenderTarget->SetDepthStencilClearValue({ 1, 0 });
        // add material system
        mRenderTarget->AddMaterialSystem<ade::AdBaseMaterialSystem>();
        mRenderTarget->AddMaterialSystem<ade::AdPhongMaterialSystem>();
        mRenderTarget->AddMaterialSystem<ade::AdPBRMaterialSystem>();


        mRenderer = std::make_shared<ade::AdRenderer>();

        mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffer(swapchain->GetImages().size());

        std::vector<ade::AdVertex> vertices;
        std::vector<uint32_t> indices;
        ade::AdGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
        mCubeMesh = std::make_shared<ade::AdMesh>(vertices, indices);
        mDefaultSampler = std::make_shared<ade::AdSampler>(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
        ade::RGBAColor multiColors[4] = {
            255, 255, 255, 255,
            192, 192, 192, 255,
            192, 192, 192, 255,
            255, 255, 255, 255
        };
        mMultiPixelTexture = std::make_shared<ade::AdTexture>(2, 2, multiColors);

        // init imgui
        AdEditorApp::OnInit();
        //mSceneRenderTargets = std::vector<std::shared_ptr<ade::AdRenderTarget>>{mRenderTarget};
        //uint32_t index;
        //AddViewportWindow(mRenderPass.get(),mRenderTarget->GetCamera(),&index);
    }

    void OnSceneInit(ade::AdScene *scene) override {
        ade::AdEntity *camera = scene->CreateEntity("Editor Camera");
        auto &cameraComp = camera->AddComponent<ade::AdLookAtCameraComponent>();
        cameraComp.SetRadius(2.f);
        mRenderTarget->SetCamera(camera);

        mTestModel = std::make_shared<ade::AdModel>(AD_RES_MODEL_DIR"lisa/Lisa.obj");
        mTexture0 = std::make_shared<ade::AdTexture>(AD_RES_TEXTURE_DIR"R-C.jpeg");
        mSphereModel = std::make_shared<ade::AdModel>(AD_RES_MODEL_DIR"sphere.gltf");
        auto baseMat0 = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdBaseMaterial>();
        baseMat0->colorType = ade::COLOR_TYPE_NORMAL;
        auto baseMat1 = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdBaseMaterial>();
        baseMat1->colorType = ade::COLOR_TYPE_TEXCOORD;
        auto phong = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdPhongMaterial>();
        phong->SetBaseColor0(glm::linearRand(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f)));
        phong->SetTextureView(0, mMultiPixelTexture.get(), mDefaultSampler.get());

        auto pbrMat = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdPBRMaterial>();
        pbrMat->SetPBRMaterialUbo(ade::PBRMaterialUbo(0.4,0.2,glm::vec3(0.4f, 0.2f, 0.5f)));
        // 1 shader, 2 component, 3 system
        {
            ade::AdEntity *lisa = scene->CreateEntity("Lisa");
            // ??????
            auto pairs = mTestModel->mMeshTextureMap;
            auto &materialComp = lisa->AddComponent<ade::AdPhongMaterialComponent>();
            for (auto meshTexture : pairs) {
                auto phongLisa = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdPhongMaterial>();
                phongLisa->SetBaseColor0(glm::linearRand(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f)));
                phongLisa->SetLightParam0(glm::vec3(0,-5,-5));
                phongLisa->SetTextureView(0,mTestModel->mMaterials[meshTexture.second].get(),mDefaultSampler.get());
                materialComp.AddMesh(meshTexture.first.get(), phongLisa);
            }
            auto &transComp = lisa->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({ 0.1f, 0.1f, 0.1f });
            transComp.SetPosition({ 0.f, -0.8f, 0.0f });
            transComp.SetRotation( { 0.f, 0.f, 0.f });
        }
        {
            ade::AdEntity *cube = scene->CreateEntity("Cube 1");
            auto &materialComp = cube->AddComponent<ade::AdPhongMaterialComponent>();
            materialComp.AddMesh(mCubeMesh.get(), phong);
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({ 0.5f, 0.5f, 0.5f });
            transComp.SetPosition({ -1.f, 0.f, 0.0f });
            transComp.SetRotation( { 0.f, 0.f, 0.f });
        }
        {
            ade::AdEntity *cube = scene->CreateEntity("Cube 2");
            auto &materialComp = cube->AddComponent<ade::AdPhongMaterialComponent>();
            materialComp.AddMesh(mCubeMesh.get(), phong);
            phong->SetTextureView(0, mTexture0.get(), mDefaultSampler.get());
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({ 0.5f, 0.5f, 0.5f });
            transComp.SetPosition({ -0.5f, -1.f, 0.0f });
            transComp.SetRotation( { 0.f, 0.f, 0.f });
        }
        {
            ade::AdEntity *cube = scene->CreateEntity("Cube 3");
            auto &materialComp = cube->AddComponent<ade::AdPBRMaterialComponent>();
            materialComp.AddMesh(mSphereModel.get()->GetMeshes(0)[0], pbrMat);
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({ 0.3f, 0.3f, 0.3f });
            transComp.SetPosition({ 0.5f, -1.f, 0.0f });
            transComp.SetRotation( { 0.f, 0.f, 0.f });
        }
        {
            ade::AdEntity *cube = scene->CreateEntity("Cube 4");
            auto &materialComp = cube->AddComponent<ade::AdPBRMaterialComponent>();
            materialComp.AddMesh(mCubeMesh.get(), pbrMat);
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({ 0.5f, 0.5f, 0.5f });
            transComp.SetPosition({ 1.0f, 0.0f, 0.0f });
            transComp.SetRotation( { 17.f, 30.f, 0.f });
        }
    }

    void OnSceneDestroy(ade::AdScene *scene) override {

    }

    void OnUpdate(float deltaTime) override {
        ade::AdEntity *camera = mRenderTarget->GetCamera();
        if(ade::AdEntity::HasComponent<ade::AdLookAtCameraComponent>(camera)) {
            auto &cameraComp = camera->GetComponent<ade::AdLookAtCameraComponent>();
            cameraComp.UpdateCamera(deltaTime);
        }

    }
    void OnRender() override {

        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();


        if(mRenderer->Begin(&imageIndex)){
            mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
            mGuiRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
        }

        VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
        ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffer);
        mRenderTarget->Begin(cmdBuffer);
        mRenderTarget->RenderMaterialSystems(cmdBuffer);
        mRenderTarget->End(cmdBuffer);
        ade::AdVKCommandPool::EndCommandBuffer(cmdBuffer);

        AdEditorApp::OnRender();

        //  swapchain end
        if(mRenderer->End(imageIndex, { cmdBuffer,mGuiCmdBuffers[imageIndex] })){
            mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
            mGuiRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
        }
    }


    void OnDestroy() override {
        ade::AdRenderContext *renderCxt = ade::AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        vkDeviceWaitIdle(device->GetHandle());
        mCubeMesh.reset();
        mCmdBuffers.clear();
        mRenderTarget.reset();
        mRenderPass.reset();
        mRenderer.reset();
        mMultiPixelTexture.reset();
        mDefaultSampler.reset();
        mTestModel.reset();
        mTexture0.reset();
        mObserver.reset();
        mSphereModel.reset();
        AdEditorApp::OnDestroy();
    }
private:
    std::shared_ptr<ade::AdVKRenderPass> mRenderPass;
    std::shared_ptr<ade::AdRenderTarget> mRenderTarget;
    std::shared_ptr<ade::AdRenderer> mRenderer;
    std::vector<VkCommandBuffer> mCmdBuffers;
    std::shared_ptr<ade::AdMesh> mCubeMesh;
    std::shared_ptr<ade::AdSampler> mDefaultSampler;
    std::shared_ptr<ade::AdModel> mTestModel;
    std::shared_ptr<ade::AdModel> mSphereModel;

    std::shared_ptr<ade::AdTexture> mMultiPixelTexture;
    std::shared_ptr<ade::AdTexture> mTexture0;



};

ade::AdApplication *CreateApplicationEntryPoint(){
    return new MainRenderApp();
}