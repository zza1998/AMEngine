#include "AdEntryPoint.h"
#include "AdFileUtil.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKCommandBuffer.h"

#include "Render/AdRenderTarget.h"
#include "Render/AdMesh.h"
#include "Render/AdRenderer.h"
#include "Render/AdMaterial.h"

#include "ECS/AdEntity.h"
#include "ECS/System/AdBaseMaterialSystem.h"
#include "ECS/System/AdUnlitMaterialSystem.h"
#include "ECS/Component/AdLookAtCameraComponent.h"

#include "AdEventTesting.h"

class SandBoxApp : public ade::AdApplication{
protected:
    void OnConfiguration(ade::AppSettings *appSettings) override {
        appSettings->width = 1360;
        appSettings->height = 768;
        appSettings->title = "SandBox";
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
        mRenderTarget->AddMaterialSystem<ade::AdBaseMaterialSystem>();
        mRenderTarget->AddMaterialSystem<ade::AdUnlitMaterialSystem>();

        mRenderer = std::make_shared<ade::AdRenderer>();

        mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffer(swapchain->GetImages().size());

        // Event
        mEventTesting = std::make_shared<AdEventTesting>();
        mObserver = std::make_shared<ade::AdEventObserver>();
        mObserver->OnEvent<ade::AdMouseScrollEvent>([this](const ade::AdMouseScrollEvent &event){
            ade::AdEntity *camera = mRenderTarget->GetCamera();
            if(ade::AdEntity::HasComponent<ade::AdLookAtCameraComponent>(camera)){
                auto &cameraComp = camera->GetComponent<ade::AdLookAtCameraComponent>();
                float radius = cameraComp.GetRadius() + event.mYOffset * -0.3f;
                if(radius < 0.1f){
                    radius = 0.1f;
                }
                cameraComp.SetRadius(radius);
            }
        });

        std::vector<ade::AdVertex> vertices;
        std::vector<uint32_t> indices;
        ade::AdGeometryUtil::CreateCube(-0.1f, 0.1f, -0.1f, 0.1f, -0.1f, 0.1f, vertices, indices);
        mCubeMesh = std::make_shared<ade::AdMesh>(vertices, indices);

        mDefaultSampler = std::make_shared<ade::AdSampler>(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
        ade::RGBAColor whiteColor = { 255, 255, 255, 255 };
        ade::RGBAColor blackColor = { 0, 0, 0, 0 };
        ade::RGBAColor multiColors[4] = {
            255, 255, 255, 255,
            192, 192, 192, 255,
            192, 192, 192, 255,
            255, 255, 255, 255
        };
        mWhiteTexture = std::make_shared<ade::AdTexture>(1, 1, &whiteColor);
        mBlackTexture = std::make_shared<ade::AdTexture>(1, 1, &blackColor);
        mMultiPixelTexture = std::make_shared<ade::AdTexture>(2, 2, multiColors);
        mFileTexture = std::make_shared<ade::AdTexture>(AD_RES_TEXTURE_DIR"R-C.jpeg");
    }

    void OnSceneInit(ade::AdScene *scene) override {
        ade::AdEntity *camera = scene->CreateEntity("Editor Camera");
        camera->AddComponent<ade::AdLookAtCameraComponent>();
        mRenderTarget->SetCamera(camera);

        float x = -2.f;
        for(int i = 0; i < mSmallCubeSize.x; i++, x+=0.5f){
            float y = -2.f;
            for(int j = 0; j < mSmallCubeSize.y; j++, y+=0.5f){
                float z = -2.f;
                for(int k = 0; k < mSmallCubeSize.z; k++, z+=0.5f){
                    ade::AdEntity *cube = scene->CreateEntity("Cube");
                    auto &transComp = cube->GetComponent<ade::AdTransformComponent>();
                    transComp.position = { x, y, z };
                    mSmallCubes.push_back(cube);
                }
            }
        }
    }

    void OnSceneDestroy(ade::AdScene *scene) override {

    }

    void OnUpdate(float deltaTime) override {
        uint64_t frameIndex = GetFrameIndex();

        ade::AdTexture *textures[] = { mWhiteTexture.get(), mBlackTexture.get(), mMultiPixelTexture.get(), mFileTexture.get() };
        if(frameIndex % 2 == 0 && mUnlitMaterials.size() < mSmallCubes.size()){
            auto material = ade::AdMaterialFactory::GetInstance()->CreateMaterial<ade::AdUnlitMaterial>();
            material->SetBaseColor0(glm::linearRand(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f)));
            material->SetBaseColor1(glm::linearRand(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f)));
            material->SetTextureView(ade::UNLIT_MAT_BASE_COLOR_0, textures[glm::linearRand(0, (int)ARRAY_SIZE(textures) - 1)], mDefaultSampler.get());
            material->SetTextureView(ade::UNLIT_MAT_BASE_COLOR_1, textures[glm::linearRand(0, (int)ARRAY_SIZE(textures) - 1)], mDefaultSampler.get());
            material->UpdateTextureViewEnable(ade::UNLIT_MAT_BASE_COLOR_0, glm::linearRand(0, 1));
            material->UpdateTextureViewEnable(ade::UNLIT_MAT_BASE_COLOR_1, glm::linearRand(0, 1));
            material->SetMixValue(glm::linearRand(0.1f, 0.8f));

            uint32_t cubeIndex = mUnlitMaterials.size();
            if(!ade::AdEntity::HasComponent<ade::AdUnlitMaterialComponent>(mSmallCubes[cubeIndex])){
                mSmallCubes[cubeIndex]->AddComponent<ade::AdUnlitMaterialComponent>();
            }
            auto &materialComp = mSmallCubes[cubeIndex]->GetComponent<ade::AdUnlitMaterialComponent>();
            materialComp.AddMesh(mCubeMesh.get(), material);

            mUnlitMaterials.push_back(material);
            LOG_D("Unlit Material Count: {0}", mUnlitMaterials.size());
        }

        if(frameIndex % 20 == 0 && !mUnlitMaterials.empty()){
            mUnlitMaterials[0]->SetMixValue(glm::linearRand(0.f, 1.f));
            mUnlitMaterials[0]->SetTextureView(ade::UNLIT_MAT_BASE_COLOR_0, textures[glm::linearRand(0, (int)ARRAY_SIZE(textures) - 1)], mDefaultSampler.get());
        }

        ade::AdEntity *camera = mRenderTarget->GetCamera();
        if(ade::AdEntity::HasComponent<ade::AdLookAtCameraComponent>(camera)){
            if(!mWindow->IsMouseDown()){
                bFirstMouseDrag = true;
                return;
            }

            glm::vec2 mousePos;
            mWindow->GetMousePos(mousePos);
            glm::vec2 mousePosDelta = { mLastMousePos.x - mousePos.x, mousePos.y - mLastMousePos.y };
            mLastMousePos = mousePos;

            if(abs(mousePosDelta.x) > 0.1f || abs(mousePosDelta.y) > 0.1f){
                if(bFirstMouseDrag){
                    bFirstMouseDrag = false;
                } else {
                    auto &transComp = camera->GetComponent<ade::AdTransformComponent>();
                    float yaw = transComp.rotation.x;
                    float pitch = transComp.rotation.y;

                    yaw += mousePosDelta.x * mMouseSensitivity;
                    pitch += mousePosDelta.y * mMouseSensitivity;

                    if(pitch > 89.f){
                        pitch = 89.f;
                    }
                    if(pitch < -89.f){
                        pitch = -89.f;
                    }
                    transComp.rotation.x = yaw;
                    transComp.rotation.y = pitch;
                }
            }
        }
    }

    void OnRender() override {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKSwapchain *swapchain = renderCxt->GetSwapchain();

        int32_t imageIndex;
        if(mRenderer->Begin(&imageIndex)){
            mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
        }

        VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
        ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffer);

        mRenderTarget->Begin(cmdBuffer);
        mRenderTarget->RenderMaterialSystems(cmdBuffer);
        mRenderTarget->End(cmdBuffer);

        ade::AdVKCommandPool::EndCommandBuffer(cmdBuffer);
        if(mRenderer->End(imageIndex, { cmdBuffer })){
            mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
        }
    }

    void OnDestroy() override {
        ade::AdRenderContext *renderCxt = ade::AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        vkDeviceWaitIdle(device->GetHandle());
        mDefaultSampler.reset();
        mWhiteTexture.reset();
        mBlackTexture.reset();
        mMultiPixelTexture.reset();
        mFileTexture.reset();
        mCubeMesh.reset();
        mCmdBuffers.clear();
        mRenderTarget.reset();
        mRenderPass.reset();
        mRenderer.reset();
    }
private:
    std::shared_ptr<ade::AdVKRenderPass> mRenderPass;
    std::shared_ptr<ade::AdRenderTarget> mRenderTarget;
    std::shared_ptr<ade::AdRenderer> mRenderer;

    std::vector<VkCommandBuffer> mCmdBuffers;

    std::shared_ptr<ade::AdMesh> mCubeMesh;
    std::shared_ptr<ade::AdTexture> mWhiteTexture;
    std::shared_ptr<ade::AdTexture> mBlackTexture;
    std::shared_ptr<ade::AdTexture> mMultiPixelTexture;
    std::shared_ptr<ade::AdTexture> mFileTexture;
    std::shared_ptr<ade::AdSampler> mDefaultSampler;
    glm::ivec3 mSmallCubeSize{ 10, 10, 10 }; // x, y, z
    std::vector<ade::AdEntity*> mSmallCubes;
    std::vector<ade::AdUnlitMaterial*> mUnlitMaterials;

    std::shared_ptr<AdEventTesting> mEventTesting;
    std::shared_ptr<ade::AdEventObserver> mObserver;

    bool bFirstMouseDrag = true;
    glm::vec2 mLastMousePos;
    float mMouseSensitivity = 0.25f;
};

ade::AdApplication *CreateApplicationEntryPoint(){
    return new SandBoxApp();
}