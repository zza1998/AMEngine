#include <iostream>
#include "AdLog.h"
#include "AdFileUtil.h"
#include "AdGeometryUtil.h"
#include "AdWindow.h"
#include "AdGraphicContext.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKQueue.h"
#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVkSwapchain.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKFrameBuffer.h"
#include "Graphic/AdVKPipeline.h"
#include "Graphic/AdVKCommandBuffer.h"
#include "Graphic/AdVKImage.h"
#include "Graphic/AdVKBuffer.h"

int main(){
	std::cout << __cplusplus << std::endl;
    std::cout << "Hello adiosy engine." << std::endl;

    ade::AdLog::Init();
    LOG_T("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_D("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_I("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_W("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_E("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);

    std::unique_ptr<ade::AdWindow> window = ade::AdWindow::Create(800, 600, "00_FirstTriangle");
    std::unique_ptr<ade::AdGraphicContext> graphicContext = ade::AdGraphicContext::Create(window.get());
    auto vkContext = dynamic_cast<ade::AdVKGraphicContext*>(graphicContext.get());
    std::shared_ptr<ade::AdVKDevice> device = std::make_shared<ade::AdVKDevice>(vkContext, 1, 1);
    std::shared_ptr<ade::AdVKSwapchain> swapchain = std::make_shared<ade::AdVKSwapchain>(vkContext, device.get());
    swapchain->ReCreate();

    std::shared_ptr<ade::AdVKRenderPass> renderPass = std::make_shared<ade::AdVKRenderPass>(device.get());

    std::vector<VkImage> swapchainImages = swapchain->GetImages();
    uint32_t swapchainImageSize = swapchainImages.size();
    std::vector<std::shared_ptr<ade::AdVKFrameBuffer>> framebuffers;
    VkExtent3D imageExtent = { swapchain->GetWidth(), swapchain->GetHeight(), 1 };
    for(int i = 0; i < swapchainImageSize; i++){
        std::vector<std::shared_ptr<ade::AdVKImage>> images = {
            std::make_shared<ade::AdVKImage>(device.get(), swapchainImages[i], imageExtent, device->GetSettings().surfaceFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
        };
        framebuffers.push_back(std::make_shared<ade::AdVKFrameBuffer>(device.get(), renderPass.get(), images, swapchain->GetWidth(), swapchain->GetHeight()));
    }

    std::shared_ptr<ade::AdVKPipelineLayout> pipelineLayout = std::make_shared<ade::AdVKPipelineLayout>(device.get(),
                                                                                                        AD_RES_SHADER_DIR"00_hello_triangle.vert",
                                                                                                        AD_RES_SHADER_DIR"00_hello_triangle.frag");
    std::shared_ptr<ade::AdVKPipeline> pipeline = std::make_shared<ade::AdVKPipeline>(device.get(), renderPass.get(), pipelineLayout.get());
    pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
    pipeline->Create();

    std::shared_ptr<ade::AdVKCommandPool> cmdPool = std::make_shared<ade::AdVKCommandPool>(device.get(), vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
    std::vector<VkCommandBuffer> cmdBuffers = cmdPool->AllocateCommandBuffer(swapchainImages.size());

    ade::AdVKQueue *graphicQueue = device->GetFirstGraphicQueue();
    const std::vector<VkClearValue> clearValues = {
            { .color = { 0.1f, 0.2f, 0.3f, 1.f }}, { .depthStencil = { 1, 0 } }
    };

    VkFence acquireFence;
    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0

    };
    CALL_VK(vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &acquireFence));

    while (!window->ShouldClose()){
        window->PollEvents();

        //1. acquire swapchain image
        int32_t imageIndex;
        swapchain->AcquireImage(&imageIndex, VK_NULL_HANDLE, acquireFence);
        //LOG_D("{0}", imageIndex);

        //2. begin cmdbuffer
        ade::AdVKCommandPool::BeginCommandBuffer(cmdBuffers[imageIndex]);

        //3. begin renderpass, bind framebuffer
        renderPass->Begin(cmdBuffers[imageIndex], framebuffers[imageIndex].get(), clearValues);

        //4. bind resource -> pipeline、geometry、descriptorset...
        pipeline->Bind(cmdBuffers[imageIndex]);

        VkViewport viewport = {
                .x = 0,
                .y = 0,
                .width = static_cast<float>(framebuffers[imageIndex]->GetWidth()),
                .height = static_cast<float>(framebuffers[imageIndex]->GetHeight()),
                .minDepth = 0.f,
                .maxDepth = 1.f
        };
        vkCmdSetViewport(cmdBuffers[imageIndex], 0, 1, &viewport);
        VkRect2D scissor = {
                .offset = { 0, 0 },
                .extent = { framebuffers[imageIndex]->GetWidth(), framebuffers[imageIndex]->GetHeight() }
        };
        vkCmdSetScissor(cmdBuffers[imageIndex], 0, 1, &scissor);

        //5. draw
        vkCmdDraw(cmdBuffers[imageIndex], 3, 1, 0, 0);

        //6. end renderpass
        renderPass->End(cmdBuffers[imageIndex]);
        //7. end cmdbuffer
        ade::AdVKCommandPool::EndCommandBuffer(cmdBuffers[imageIndex]);
        //8. submit cmdbuffer to queue
        graphicQueue->Submit({ cmdBuffers[imageIndex] }, {  }, {  });
        graphicQueue->WaitIdle();

        //9. present
        swapchain->Present(imageIndex, {  });

        window->SwapBuffer();
    }

    vkDeviceWaitIdle(device->GetHandle());
    VK_D(Fence, device->GetHandle(), acquireFence);

    return EXIT_SUCCESS;
}