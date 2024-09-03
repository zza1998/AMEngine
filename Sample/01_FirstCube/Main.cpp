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

struct PushConstants{
    glm::mat4 matrix{ 1.f };
    uint32_t colorType = 0;
};

int main(){

    std::cout << "Hello adiosy engine." << std::endl;

    ade::AdLog::Init();
    LOG_T("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_D("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_I("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_W("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);
    LOG_E("Hello spdlog: {0}, {1}, {3}", __FUNCTION__, 1, 0.14f, true);

    std::unique_ptr<ade::AdWindow> window = ade::AdWindow::Create(800, 600, "01_FirstCube");
    std::unique_ptr<ade::AdGraphicContext> graphicContext = ade::AdGraphicContext::Create(window.get());
    auto vkContext = dynamic_cast<ade::AdVKGraphicContext*>(graphicContext.get());
    std::shared_ptr<ade::AdVKDevice> device = std::make_shared<ade::AdVKDevice>(vkContext, 1, 1);
    std::shared_ptr<ade::AdVKSwapchain> swapchain = std::make_shared<ade::AdVKSwapchain>(vkContext, device.get());
    swapchain->ReCreate();

    // TODO query depth format
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

    // render pass
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
                    .format = depthFormat,
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
                    .sampleCount = VK_SAMPLE_COUNT_1_BIT
            }
    };
    std::shared_ptr<ade::AdVKRenderPass> renderPass = std::make_shared<ade::AdVKRenderPass>(device.get(), attachments, subpasses);

    std::vector<VkImage> swapchainImages = swapchain->GetImages();
    uint32_t swapchainImageSize = swapchainImages.size();
    std::vector<std::shared_ptr<ade::AdVKFrameBuffer>> framebuffers;
    VkExtent3D imageExtent = { swapchain->GetWidth(), swapchain->GetHeight(), 1 };
    for(int i = 0; i < swapchainImageSize; i++){
        std::vector<std::shared_ptr<ade::AdVKImage>> images = {
            std::make_shared<ade::AdVKImage>(device.get(), swapchainImages[i], imageExtent, device->GetSettings().surfaceFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
            std::make_shared<ade::AdVKImage>(device.get(), imageExtent, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        };
        framebuffers.push_back(std::make_shared<ade::AdVKFrameBuffer>(device.get(), renderPass.get(), images, swapchain->GetWidth(), swapchain->GetHeight()));
    }

    ade::ShaderLayout shaderLayout = {
            .pushConstants = {
                {
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof(PushConstants)
                }
            }
    };
    std::shared_ptr<ade::AdVKPipelineLayout> pipelineLayout = std::make_shared<ade::AdVKPipelineLayout>(device.get(),
                                                                                                        AD_RES_SHADER_DIR"01_hello_buffer.vert",
                                                                                                        AD_RES_SHADER_DIR"01_hello_buffer.frag", shaderLayout);
    std::shared_ptr<ade::AdVKPipeline> pipeline = std::make_shared<ade::AdVKPipeline>(device.get(), renderPass.get(), pipelineLayout.get());
    std::vector<VkVertexInputBindingDescription> vertexBindings = {
            {
                .binding = 0,
                .stride = sizeof(ade::AdVertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            }
    };
    std::vector<VkVertexInputAttributeDescription> vertexAttrs = {
            {
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(ade::AdVertex, position)
            },
            {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(ade::AdVertex, texcoord0)
            },
            {
                .location = 2,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(ade::AdVertex, normal)
            },
    };
    pipeline->SetVertexInputState(vertexBindings, vertexAttrs);
    pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
    pipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
    pipeline->Create();

    std::shared_ptr<ade::AdVKCommandPool> cmdPool = std::make_shared<ade::AdVKCommandPool>(device.get(), vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
    std::vector<VkCommandBuffer> cmdBuffers = cmdPool->AllocateCommandBuffer(swapchainImages.size());

    // Geometry
    std::vector<ade::AdVertex> vertices;
    std::vector<uint32_t> indices;
    ade::AdGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
    std::shared_ptr<ade::AdVKBuffer> vertexBuffer = std::make_shared<ade::AdVKBuffer>(device.get(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                                      sizeof(vertices[0]) * vertices.size(), vertices.data());
    std::shared_ptr<ade::AdVKBuffer> indexBuffer = std::make_shared<ade::AdVKBuffer>(device.get(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                                                     sizeof(indices[0]) * indices.size(), indices.data());

    PushConstants pc{};

    ade::AdVKQueue *graphicQueue = device->GetFirstGraphicQueue();
    const std::vector<VkClearValue> clearValues = {
            { .color = { 0.1f, 0.2f, 0.3f, 1.f }}, { .depthStencil = { 1, 0 } }
    };

    //2
    //1. acquire swapchain image Semaphore
    //2. submited Semaphore
    //3. frame fence
    const uint32_t numBuffer = 2;
    std::vector<VkSemaphore> imageAvailableSemaphores(numBuffer);
    std::vector<VkSemaphore> submitedSemaphores(numBuffer);
    std::vector<VkFence> frameFences(numBuffer);

    VkSemaphoreCreateInfo semaphoreInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
    };
    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT

    };
    for(int i = 0; i < numBuffer; i++){
        CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
        CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &submitedSemaphores[i]));
        CALL_VK(vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &frameFences[i]));
    }

    std::chrono::time_point lastTimePoint = std::chrono::steady_clock::now();

    uint32_t currentBuffer = 0;
    while (!window->ShouldClose()){
        window->PollEvents();

        CALL_VK(vkWaitForFences(device->GetHandle(), 1, &frameFences[currentBuffer], VK_TRUE, UINT64_MAX));
        CALL_VK(vkResetFences(device->GetHandle(), 1, &frameFences[currentBuffer]));

        //1. acquire swapchain image
        int32_t imageIndex;
        CALL_VK(swapchain->AcquireImage(&imageIndex, imageAvailableSemaphores[currentBuffer]));

        float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - lastTimePoint).count();
        pc.matrix = glm::rotate(glm::mat4(1.f), glm::radians(-17.f), glm::vec3(1, 0, 0));
        pc.matrix = glm::rotate(pc.matrix, glm::radians(time * 100.f), glm::vec3(0, 1, 0));

        pc.matrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f) * pc.matrix;

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

        vkCmdPushConstants(cmdBuffers[imageIndex], pipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), &pc);

        // Bind buffers
        VkBuffer vertexBuffers[] = { vertexBuffer->GetHandle() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmdBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(cmdBuffers[imageIndex], indexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);

        //5. draw
        //vkCmdDraw(cmdBuffers[imageIndex], 3, 1, 0, 0);
        vkCmdDrawIndexed(cmdBuffers[imageIndex], indices.size(), 1, 0, 0, 0);

        //6. end renderpass
        renderPass->End(cmdBuffers[imageIndex]);
        //7. end cmdbuffer
        ade::AdVKCommandPool::EndCommandBuffer(cmdBuffers[imageIndex]);
        //8. submit cmdbuffer to queue
        graphicQueue->Submit({ cmdBuffers[imageIndex] }, { imageAvailableSemaphores[currentBuffer] }, { submitedSemaphores[currentBuffer] }, frameFences[currentBuffer]);
        //graphicQueue->WaitIdle();

        //9. present
        swapchain->Present(imageIndex, { submitedSemaphores[currentBuffer] });

        window->SwapBuffer();
        currentBuffer = (currentBuffer + 1) % numBuffer;
    }

    for(int i = 0; i < numBuffer; i++){
        vkDeviceWaitIdle(device->GetHandle());
        VK_D(Semaphore, device->GetHandle(), imageAvailableSemaphores[i]);
        VK_D(Semaphore, device->GetHandle(), submitedSemaphores[i]);
        VK_D(Fence, device->GetHandle(), frameFences[i]);
    }
    return EXIT_SUCCESS;
}