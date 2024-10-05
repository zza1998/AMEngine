#include <AdEditorApp.h>
#include <ECS/Component/AdSkyBoxComponent.h>
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

class MainRenderApp : public ade::AdEditorApp {
protected:
    void OnConfiguration(ade::AppSettings *appSettings) override {
        appSettings->width = 1360;
        appSettings->height = 768;
        appSettings->title = "MainRender";
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
            },
            {
                .format = swapchain->GetSurfaceInfo().surfaceFormat.format,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
            },
            {
                .format = swapchain->GetSurfaceInfo().surfaceFormat.format,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
            },
            {
                .format = device->GetSettings().depthFormat,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
            },{
                .format = swapchain->GetSurfaceInfo().surfaceFormat.format,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
            },

        };
        std::vector<ade::RenderSubPass> subpasses = {
            {
                .colorAttachments = {0, 1, 2},
                .depthStencilAttachments = {3},
                .sampleCount = VK_SAMPLE_COUNT_1_BIT
            },
            {
                .inputAttachments = {0,1,2,3},
                .colorAttachments = {4},
                .sampleCount = VK_SAMPLE_COUNT_4_BIT
            }
        };
        mRenderPass = std::make_shared<ade::AdVKRenderPass>(
            device, attachments, subpasses);


        mRenderTarget = std::make_shared<ade::AdRenderTarget>(mRenderPass.get());
        mRenderTarget->SetColorClearValue({0.1f, 0.2f, 0.3f, 1.f});
        mRenderTarget->SetDepthStencilClearValue({1, 0});
        mRenderTarget->AddGBufferRenderSystem();
        mRenderTarget->AddLightRenderSystem();
        // add material system
        //mRenderTarget->AddMaterialSystem<ade::AdBaseMaterialSystem>();
        //mRenderTarget->AddMaterialSystem<ade::AdPhongMaterialSystem>();
        mRenderTarget->AddMaterialSystem<ade::AdPBRMaterialSystem>();
        //mRenderTarget->AddSkyBoxSystem();
    }

    void OnInit() override {
        // init imgui
        AdEditorApp::OnInit();
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        buildGbufferPass();

        mRenderer = std::make_shared<ade::AdRenderer>();

        mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffer(swapchain->GetImages().size());

        std::vector<ade::AdVertex> vertices;
        std::vector<uint32_t> indices;
        ade::AdGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
        mCubeMesh = std::make_shared<ade::AdMesh>(vertices, indices);
        mDefaultSampler = std::make_shared<ade::AdSampler>(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        ade::RGBAColor multiColors[4] = {
            255, 255, 255, 255,
            192, 192, 192, 255,
            192, 192, 192, 255,
            255, 255, 255, 255
        };
        mMultiPixelTexture = std::make_shared<ade::AdTexture>(2, 2, multiColors);
    }

    void OnSceneInit(ade::AdScene *scene) override {
        // Camera
        ade::AdEntity *camera = scene->CreateEntity("Editor Camera");
        auto &cameraComp = camera->AddComponent<ade::AdLookAtCameraComponent>();
        cameraComp.SetRadius(2.f);
        mRenderTarget->SetCamera(camera);

        // Light
        ade::AdEntity *pointLight = scene->CreateEntity("PointLight0");
        auto &pointLightComp = pointLight->AddComponent<ade::AdPointLightComponent>();
        pointLightComp.params.position = glm::vec3(0, -1, -1);
        pointLightComp.params.diffuse = glm::vec3(0.8, 0.5, 0.7);

        //skybox
        ade::AdEntity *skyBox = scene->CreateEntity("SkyBox");
        auto &skyBoxComp = skyBox->AddComponent<ade::AdSkyBoxComponent>();
        skyBoxComp.SetTexture(
            std::make_shared<ade::AdCubeTexture>(AD_RES_TEXTURE_DIR"gcanyon_cube.ktx", VK_FORMAT_R16G16B16A16_SFLOAT));
        skyBoxComp.SetSkyBoxCube(mCubeMesh);

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


        // 1 shader, 2 component, 3 system
        {
            ade::AdEntity *lisa = scene->CreateEntity("Lisa");
            // ??????
            auto pairs = mTestModel->mMeshTextureMap;
            auto &materialComp = lisa->AddComponent<ade::AdPhongMaterialComponent>();
            for (auto meshTexture: pairs) {
                auto phongLisa = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdPhongMaterial>();
                phongLisa->SetBaseColor0(glm::linearRand(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f)));
                phongLisa->SetTextureView(0, mTestModel->mMaterials[meshTexture.second].get(), mDefaultSampler.get());
                materialComp.AddMesh(meshTexture.first.get(), phongLisa);
            }
            auto &transComp = lisa->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({0.1f, 0.1f, 0.1f});
            transComp.SetPosition({0.f, -0.8f, 0.0f});
            transComp.SetRotation({0.f, 0.f, 0.f});
        } {
            ade::AdEntity *cube = scene->CreateEntity("Cube 1");
            auto pbrMat = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdPBRMaterial>();
            pbrMat->SetPBRMaterialUbo(ade::PBRMaterialUbo(0.4, 0.3, glm::vec3(0.8f, 0.2f, 0.5f)));
            auto &materialComp = cube->AddComponent<ade::AdPBRMaterialComponent>();
            materialComp.AddMesh(mSphereModel.get()->GetMeshes(0)[0], pbrMat);
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({0.3f, 0.3f, 0.3f});
            transComp.SetPosition({-1.f, 0.f, 0.0f});
            transComp.SetRotation({0.f, 0.f, 0.f});
        } {
            ade::AdEntity *cube = scene->CreateEntity("Cube Light");
            auto &materialComp = cube->AddComponent<ade::AdBaseMaterialComponent>();
            materialComp.AddMesh(mCubeMesh.get(), baseMat0);
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({0.2f, 0.2f, 0.2f});
            transComp.SetPosition({0.0f, -1.0f, -1.0f});
            transComp.SetRotation({0.f, 0.f, 0.f});
        } {
            ade::AdEntity *cube = scene->CreateEntity("Cube 0");
            auto pbrMat = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdPBRMaterial>();
            pbrMat->SetPBRMaterialUbo(ade::PBRMaterialUbo(0.4, 0.3, glm::vec3(0.8f, 0.3f, 0.5f)));
            auto &materialComp = cube->AddComponent<ade::AdPBRMaterialComponent>();
            materialComp.AddMesh(mSphereModel.get()->GetMeshes(0)[0], pbrMat);
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({0.3f, 0.3f, 0.3f});
            transComp.SetPosition({-0.5f, -1.f, 0.0f});
            transComp.SetRotation({0.f, 0.f, 0.f});
        }
        /*{
            ade::AdEntity *cube = scene->CreateEntity("Cube 2");
            auto &materialComp = cube->AddComponent<ade::AdPhongMaterialComponent>();
            materialComp.AddMesh(mCubeMesh.get(), phong);
            phong->SetTextureView(0, mTexture0.get(), mDefaultSampler.get());
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({ 0.5f, 0.5f, 0.5f });
            transComp.SetPosition({ -0.5f, -1.f, 0.0f });
            transComp.SetRotation( { 0.f, 0.f, 0.f });
        }*/
        {
            ade::AdEntity *cube = scene->CreateEntity("Sphere 3");
            auto pbrMat = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdPBRMaterial>();
            pbrMat->SetPBRMaterialUbo(ade::PBRMaterialUbo(0.4, 0.3, glm::vec3(0.8f, 0.2f, 0.5f)));
            auto &materialComp = cube->AddComponent<ade::AdPBRMaterialComponent>();
            materialComp.AddMesh(mSphereModel.get()->GetMeshes(0)[0], pbrMat);
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({0.3f, 0.3f, 0.3f});
            transComp.SetPosition({0.5f, -1.f, 0.0f});
            transComp.SetRotation({0.f, 0.f, 0.f});
        } {
            ade::AdEntity *cube = scene->CreateEntity("Cube 4");
            auto pbrMat = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdPBRMaterial>();
            pbrMat->SetPBRMaterialUbo(ade::PBRMaterialUbo(0.4, 0.3, glm::vec3(0.4f, 0.2f, 0.5f)));
            auto &materialComp = cube->AddComponent<ade::AdPBRMaterialComponent>();
            materialComp.AddMesh(mCubeMesh.get(), pbrMat);
            auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
            transComp.SetScale({0.5f, 0.5f, 0.5f});
            transComp.SetPosition({1.0f, 0.0f, 0.0f});
            transComp.SetRotation({17.f, 30.f, 0.f});
        }
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
        mRenderTarget->RenderToGbuffer(cmdBuffer);
        // render to screen
        vkCmdNextSubpass(cmdBuffer, VK_SUBPASS_CONTENTS_INLINE);
        mRenderTarget->RenderLights(cmdBuffer);
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
        mCubeMesh.reset();
        mCmdBuffers.clear();
        mRenderPass.reset();
        mRenderer.reset();
        mRenderTarget.reset();
        mMultiPixelTexture.reset();
        mDefaultSampler.reset();
        mTestModel.reset();
        mTexture0.reset();
        mObserver.reset();
        mSphereModel.reset();
        mCubeTexture.reset();
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
    std::shared_ptr<ade::AdTexture> mCubeTexture;
};

ade::AdApplication *CreateApplicationEntryPoint() {
    return new MainRenderApp();
}
