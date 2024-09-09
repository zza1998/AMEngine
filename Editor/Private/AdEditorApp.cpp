#include "AdEditorApp.h"
#include "AdLog.h"
#include "AdFileUtil.h"
#include "Gui/AdFontAwesomeIcons.h"

#include "Render/AdRenderTarget.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKSwapchain.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKFrameBuffer.h"
#include "Graphic/AdVKCommandBuffer.h"
#include "Graphic/AdVKDescriptor.h"

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

    AdEditorApp::AdEditorApp() {
        bDirectRender = false;
    }

    void AdEditorApp::OnInit() {
        mGuiRenderPass = std::make_shared<AdRenderPass>(true);
        mGuiRenderTarget = std::make_shared<AdRenderTarget>(mRenderContext.get(), mGuiRenderPass.get());
        mSceneCmdBuffers = mRenderContext->GetDefaultCommandPool()->AllocateCmdBuffer(5);
        InitImGui();
    }

    void AdEditorApp::OnRenderGui(float deltaTime) {
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        mMainWindow.Draw(&bOpenMainWindow);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        for (const auto &item: mSceneRenderTargets){
            mMainWindow.DrawViewportWindow(item.get(), deltaTime);
        }
        ImGui::PopStyleVar();

        // Rendering
        ImGui::Render();
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        if (!main_is_minimized) {
            int32_t bufIndex = mGuiRenderTarget->Begin(mFrameIndex);
            if(bufIndex >= 0){
                std::shared_ptr<ade::AdVKCommandBuffer> cmdBuffer = mGuiCmdBuffers[bufIndex];
                cmdBuffer->Begin();
                mGuiRenderTarget->GetRenderPass()->Begin(cmdBuffer->GetCmdBuffer(), mGuiRenderTarget.get());
                ImGui_ImplVulkan_RenderDrawData(main_draw_data, cmdBuffer->GetCmdBuffer());
                mGuiRenderTarget->GetRenderPass()->End(cmdBuffer->GetCmdBuffer());
                cmdBuffer->End();

                AdVKQueue *graphicQueue = mRenderContext->GetDevice()->GetFirstGraphicQueue();
                graphicQueue->Submit({cmdBuffer->GetCmdBuffer()});
                graphicQueue->WaitIdle();
            }
            mGuiRenderTarget->End();
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

        auto *glfWwindow = static_cast<GLFWwindow *>(mWindow->GetHandle());
        if(!glfWwindow){
            LOG_E("this window is not a glfw window.");
            return;
        }

        AdVKGraphicContext *context = mRenderContext->GetGraphicContext();
        AdVKDevice *device = mRenderContext->GetDevice();
        uint32_t imageCount = mRenderContext->GetSwapchain()->GetImages().size();
        std::vector<VkDescriptorPoolSize> poolSizes = {
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 },
        };
        mGuiDescriptorPool = std::make_shared<AdVKDescriptorPool>(device, 10, poolSizes);

        ImGui_ImplGlfw_InitForVulkan(glfWwindow, true);
        ImGui_ImplVulkan_InitInfo init_info{};
        init_info.Instance = context->GetInstance();
        init_info.PhysicalDevice = context->GetPhyDevice();
        init_info.Device = device->GetHandle();
        init_info.QueueFamily = context->GetGraphicQueueFamilyInfo().queueFamilyIndex;
        init_info.Queue = device->GetFirstGraphicQueue()->GetQueue();
        init_info.PipelineCache = device->GetPipelineCache();
        init_info.DescriptorPool = mGuiDescriptorPool->GetHandle();
        init_info.Subpass = 0;
        init_info.MinImageCount = imageCount;
        init_info.ImageCount = imageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = CheckGuiVKResult;
        init_info.RenderPass = mGuiRenderTarget->GetRenderPass()->GetHandle()->GetRenderPass();
        ImGui_ImplVulkan_Init(&init_info);

        mGuiCmdPool = std::make_shared<ade::AdVKCommandPool>(device, (uint32_t)context->GetGraphicQueueFamilyInfo().queueFamilyIndex);
        mGuiCmdBuffers = mGuiCmdPool->AllocateCmdBuffer(imageCount);
    }

    void AdEditorApp::MergeIconFonts() {
        ImGuiIO &io = ImGui::GetIO();
        ImFontConfig config;
        config.MergeMode = true;
        static const ImWchar iconsRanges[] = { ade::icon::IconRangeMin, ade::icon::IconRangeMax, 0 };
        io.Fonts->AddFontFromFileTTF(AD_RES_FONT_DIR"fa-regular-400.ttf", 13.f, &config, iconsRanges);
        io.Fonts->AddFontFromFileTTF(AD_RES_FONT_DIR"fa-solid-900.ttf", 13.f, &config, iconsRanges);
    }

    AdRenderTarget* AdEditorApp::AddViewportWindow(AdRenderPass *renderPass, AdNode *defaultCamera, uint32_t *outIndex) {
        std::shared_ptr<AdRenderTarget> newRenderTarget = std::make_shared<AdRenderTarget>(mRenderContext.get(), renderPass, VkExtent2D{ 100, 100 }, 1);
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

    void AdEditorApp::OnRender() {
        if(mSceneRenderTargets.empty()){
            return;
        }
        for(int i = 0; i < mSceneRenderTargets.size(); i++){
            std::shared_ptr<ade::AdVKCommandBuffer> cmdBuffer = mSceneCmdBuffers[i];
            cmdBuffer->Begin();
            AdRenderTarget *sceneRenderTarget = mSceneRenderTargets[i].get();
            sceneRenderTarget->Begin(mFrameIndex);
            sceneRenderTarget->GetRenderPass()->Begin(cmdBuffer->GetCmdBuffer(), sceneRenderTarget);
            sceneRenderTarget->GetRenderPass()->OnRenderSystems(cmdBuffer.get(), sceneRenderTarget);
            sceneRenderTarget->GetRenderPass()->End(cmdBuffer->GetCmdBuffer());
            sceneRenderTarget->End();
            cmdBuffer->End();
            ade::AdVKDevice *device = mRenderContext->GetDevice();
            ade::AdVKQueue *graphicQueue = device->GetFirstGraphicQueue();
            graphicQueue->Submit({ cmdBuffer->GetCmdBuffer() });
            graphicQueue->WaitIdle();
        }
    }
}
