#include "AdEditorApp.h"
#include "AdLog.h"
#include "AdFileUtil.h"
#include "Gui/AdFontAwesomeIcons.h"

#include "Render/AdRenderContext.h"
#include "Render/AdRenderTarget.h"
#include "Render/AdRenderer.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKCommandBuffer.h"
#include "Graphic/AdVKDescriptorSet.h"

#include "ECS/AdScene.h"

#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/implot.h"

namespace ade{
    static void CheckGuiVKResult(VkResult rt){
        if(rt != VK_SUCCESS){
            LOG_E("ImGui VK return: {0}", vk_result_string(rt));
        }
    }

    void AdEditorApp::OnInit() {
        mGuiRenderPass = std::make_shared<AdVKRenderPass>(mRenderContext->GetDevice(),std::vector<Attachment>(),std::vector<RenderSubPass>(),false);
        mGuiRenderTarget = std::make_shared<AdRenderTarget>(mGuiRenderPass.get());
        mRenderer = std::make_shared<AdRenderer>();
        mSceneCmdBuffers = mRenderContext->GetDevice()->GetDefaultCmdPool()->AllocateCommandBuffer(5);
        InitImGui();
    }

    void AdEditorApp::OnRender() {
        AdVKSwapchain *swapchain = mRenderContext->GetSwapchain();

        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        mMainWindow.Draw(&bOpenMainWindow);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        for(int i = 0; i < mSceneRenderTargets.size(); i++){
            VkCommandBuffer cmdBuffer = mSceneCmdBuffers[i];
            ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffer);

            mSceneRenderTargets[i]->Begin(cmdBuffer);
            mSceneRenderTargets[i]->RenderMaterialSystems(cmdBuffer);
            mSceneRenderTargets[i]->End(cmdBuffer);
            ade::AdVKCommandPool::EndCommandBuffer(cmdBuffer);
            ade::AdVKDevice *device = mRenderContext->GetDevice();
            ade::AdVKQueue *graphicQueue = device->GetFirstGraphicQueue();
            graphicQueue->Submit({ cmdBuffer });
            graphicQueue->WaitIdle();

            mMainWindow.DrawViewportWindow(mSceneRenderTargets[i].get());
        }
        ImGui::PopStyleVar();

        // Rendering
        ImGui::Render();
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        if (!main_is_minimized) {

            /*if(mRenderer->Begin(&imageIndex)){
                mGuiRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
            }*/
            VkCommandBuffer cmdBuffer = mGuiCmdBuffers[imageIndex];
            ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffer);
            //mGuiRenderTarget->ClearColorClearValue();
            mGuiRenderTarget->Begin(cmdBuffer);
            ImGui_ImplVulkan_RenderDrawData(main_draw_data, cmdBuffer);
            mGuiRenderTarget->End(cmdBuffer);

            ade::AdVKCommandPool::EndCommandBuffer(cmdBuffer);
            /*if(mRenderer->End(imageIndex, { cmdBuffer })){
                mGuiRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
            }*/
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }



    void AdEditorApp::OnDestroy() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        mGuiCmdBuffers.clear();
        mSceneCmdBuffers.clear();
        mGuiCmdPool.reset();
        mRenderer.reset();
        mGuiRenderTarget.reset();
        mGuiRenderPass.reset();
        mSceneRenderTargets.clear();
        mGuiDescriptorPool.reset();
        AdApplication::OnDestroy();
    }

    void AdEditorApp::InitImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGui::StyleColorsLight();
        ImGui::GetStyle().SeparatorTextBorderSize = 1.f;

        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = AD_RES_CONFIG_DIR"ImGui.ini";
        LOG_I("ImGui Init File: {0}", io.IniFilename);
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // enable keyboard controller
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // enable docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // enable viewports
        io.ConfigWindowsMoveFromTitleBarOnly = true;            // only holding the title bar will to dragging

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
            style.WindowRounding = 3.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;           //split window alpha is 1
        }

        io.Fonts->AddFontFromFileTTF(AD_RES_FONT_DIR"opensans/OpenSans-Regular.ttf", 15.0f, nullptr);
        MergeIconFonts();

        AdWindow *window = GetWindow();
        GLFWwindow *glfWwindow = static_cast<GLFWwindow *>(window->GetImplWindowPointer());
        if(!glfWwindow){
            LOG_E("this window is not a glfw window.");
            return;
        }

        AdGraphicContext *context = mRenderContext->GetGraphicContext();
        AdVKGraphicContext *vkContext = dynamic_cast<AdVKGraphicContext *>(context);
        AdVKDevice *device = mRenderContext->GetDevice();
        uint32_t imageCount = mRenderContext->GetSwapchain()->GetImages().size();
        std::vector<VkDescriptorPoolSize> poolSizes = {
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 },
        };
        mGuiDescriptorPool = std::make_shared<AdVKDescriptorPool>(device, 10, poolSizes, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

        ImGui_ImplGlfw_InitForVulkan(glfWwindow, true);
        ImGui_ImplVulkan_InitInfo init_info{};
        init_info.Instance = vkContext->GetInstance();
        init_info.PhysicalDevice = vkContext->GetPhyDevice();
        init_info.Device = device->GetHandle();
        init_info.QueueFamily = vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex;
        init_info.Queue = device->GetFirstGraphicQueue()->GetHandle();
        init_info.PipelineCache = device->GetPipelineCache();
        init_info.DescriptorPool = mGuiDescriptorPool->GetHandle();
        init_info.Subpass = 0;
        init_info.MinImageCount = imageCount;
        init_info.ImageCount = imageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = CheckGuiVKResult;
        init_info.RenderPass = mGuiRenderPass->GetHandle();
        ImGui_ImplVulkan_Init(&init_info);

        mGuiCmdPool = std::make_shared<ade::AdVKCommandPool>(device, (uint32_t)vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
        mGuiCmdBuffers = mGuiCmdPool->AllocateCommandBuffer(imageCount);
    }

    void AdEditorApp::MergeIconFonts() {
        ImGuiIO &io = ImGui::GetIO();
        ImFontConfig config;
        config.MergeMode = true;
        static const ImWchar iconsRanges[] = { ade::icon::IconRangeMin, ade::icon::IconRangeMax, 0 };
        io.Fonts->AddFontFromFileTTF(AD_RES_FONT_DIR"fa-regular-400.ttf", 13.f, &config, iconsRanges);
        io.Fonts->AddFontFromFileTTF(AD_RES_FONT_DIR"fa-solid-900.ttf", 13.f, &config, iconsRanges);
    }

    AdRenderTarget* AdEditorApp::AddViewportWindow(AdVKRenderPass *renderPass, AdEntity *defaultCamera, uint32_t *outIndex) {
        std::shared_ptr<AdRenderTarget> newRenderTarget = std::make_shared<AdRenderTarget>(renderPass, 1, VkExtent2D{ 100, 100 });
        newRenderTarget->SetCamera(defaultCamera);
        mSceneRenderTargets.push_back(newRenderTarget);
        mMainWindow.AddViewportWindow(newRenderTarget.get());
        if(outIndex){
            *outIndex = mSceneRenderTargets.size() - 1;
        }
        return newRenderTarget.get();
    }

    void AdEditorApp::RemoveViewportWindow(uint32_t index) {
        if(index <= mSceneRenderTargets.size() - 1){
            mMainWindow.RemoveViewportWindow(mSceneRenderTargets[index].get());
            mSceneRenderTargets.erase(mSceneRenderTargets.begin() + index);
        }
    }
}
