#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKFrameBuffer.h"

namespace ade{
    /*AdVKRenderPass::AdVKRenderPass(AdVKDevice *device, const std::vector<Attachment> &attachments,
        const std::vector<RenderSubPass> &subPasses): mDevice(device), mAttachments(attachments), mSubPasses(subPasses) {
        AdVKRenderPass(device,true,attachments,subPasses);
    }*/

    AdVKRenderPass::AdVKRenderPass(AdVKDevice *device,const std::vector<Attachment> &attachments, const std::vector<RenderSubPass> &subPasses, bool clearColor )
                                        : mDevice(device), mAttachments(attachments), mSubPasses(subPasses) {
        //1. default subpass and attachment
        if(mSubPasses.empty()){
            mAttachments = {{
                .format = device->GetSettings().surfaceFormat,
                .loadOp = clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = clearColor? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
            }};
            mSubPasses = {{ .colorAttachments = { 0 }, .sampleCount = VK_SAMPLE_COUNT_1_BIT }};
        }
        //2. subpasses
        std::vector<VkSubpassDescription> subpassDescriptions(mSubPasses.size());
        std::vector<VkAttachmentReference>* inputAttachmentRefs  = new std::vector<VkAttachmentReference>[mSubPasses.size()];
        std::vector<VkAttachmentReference>* colorAttachmentRefs =new std::vector<VkAttachmentReference>[mSubPasses.size()];
        std::vector<VkAttachmentReference>* depthStencilAttachmentRefs =new std::vector<VkAttachmentReference>[mSubPasses.size()];
        VkAttachmentReference* resolveAttachmentRefs = new VkAttachmentReference[mSubPasses.size()];

        for(int i = 0; i < mSubPasses.size(); i++){
            RenderSubPass subpass = mSubPasses[i];

            for (const auto &inputAttachment: subpass.inputAttachments){
                inputAttachmentRefs[i].push_back({ inputAttachment, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
            }

            for (const auto &colorAttachment: subpass.colorAttachments){
                colorAttachmentRefs[i].push_back({ colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
                mAttachments[colorAttachment].samples = subpass.sampleCount;
                if(subpass.sampleCount > VK_SAMPLE_COUNT_1_BIT){
                    mAttachments[colorAttachment].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }
            }

            for (const auto &depthStencilAttachment: subpass.depthStencilAttachments){
                depthStencilAttachmentRefs[i].push_back({ depthStencilAttachment, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL });
                mAttachments[depthStencilAttachment].samples = subpass.sampleCount;
            }

             if(subpass.sampleCount > VK_SAMPLE_COUNT_1_BIT){
                mAttachments.push_back({
                    .format = device->GetSettings().surfaceFormat,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                });
                resolveAttachmentRefs[i] = { static_cast<uint32_t>(mAttachments.size() - 1), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
            }

            subpassDescriptions[i].flags = 0;
            subpassDescriptions[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescriptions[i].inputAttachmentCount = inputAttachmentRefs[i].size();
            subpassDescriptions[i].pInputAttachments = inputAttachmentRefs[i].data();
            subpassDescriptions[i].colorAttachmentCount = colorAttachmentRefs[i].size();
            subpassDescriptions[i].pColorAttachments = colorAttachmentRefs[i].data();
            subpassDescriptions[i].pResolveAttachments = subpass.sampleCount > VK_SAMPLE_COUNT_1_BIT ? &resolveAttachmentRefs[i] : nullptr;
            subpassDescriptions[i].pDepthStencilAttachment = depthStencilAttachmentRefs[i].data();
            subpassDescriptions[i].preserveAttachmentCount = 0;
            subpassDescriptions[i].pPreserveAttachments = nullptr;
        }

        std::vector<VkSubpassDependency> dependencies(mSubPasses.size() - 1);
        if(mSubPasses.size() > 1){
            for(int i = 0; i < dependencies.size(); i++){
                dependencies[i].srcSubpass      = i;
                dependencies[i].dstSubpass      = i + 1;
                dependencies[i].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[i].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                dependencies[i].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[i].dstAccessMask   = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }
        if(!clearColor) {
            dependencies.resize(mSubPasses.size());
            for(int i = 0; i < dependencies.size(); i++){
                dependencies[i].srcSubpass      = VK_SUBPASS_EXTERNAL;
                dependencies[i].dstSubpass      = 0;
                dependencies[i].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[i].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[i].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[i].dstAccessMask   = 0;
                dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }

        //3. createinfo
        std::vector<VkAttachmentDescription> vkAttachments;
        vkAttachments.reserve(mAttachments.size());
        for (const auto &attachment: mAttachments){
            vkAttachments.push_back({
                .flags = 0,
                .format = attachment.format,
                .samples = attachment.samples,
                .loadOp = attachment.loadOp,
                .storeOp = attachment.storeOp,
                .stencilLoadOp = attachment.stencilLoadOp,
                .stencilStoreOp = attachment.stencilStoreOp,
                .initialLayout = attachment.initialLayout,
                .finalLayout = attachment.finalLayout
            });
        }
        VkRenderPassCreateInfo renderPassInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .attachmentCount = static_cast<uint32_t>(vkAttachments.size()),
                .pAttachments = vkAttachments.data(),
                .subpassCount = static_cast<uint32_t>(mSubPasses.size()),
                .pSubpasses = subpassDescriptions.data(),
                .dependencyCount = static_cast<uint32_t>(dependencies.size()),
                .pDependencies = dependencies.data()
        };
        VkResult rt = vkCreateRenderPass(mDevice->GetHandle(), &renderPassInfo, nullptr, &mHandle);
        LOG_T("RenderPass {0} : {1}, attachment count: {2}, subpass count: {3}", __FUNCTION__, (void*)mHandle, mAttachments.size(), mSubPasses.size());
    }


    AdVKRenderPass::~AdVKRenderPass() {
        VK_D(RenderPass, mDevice->GetHandle(), mHandle);
    }

    void AdVKRenderPass::Begin(VkCommandBuffer cmdBuffer, AdVKFrameBuffer *frameBuffer, const std::vector<VkClearValue> &clearValues) const {
        VkRect2D renderArea = {
                .offset = { 0, 0 },
                .extent = { frameBuffer->GetWidth(), frameBuffer->GetHeight() }
        };
        VkRenderPassBeginInfo beginInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = mHandle,
                .framebuffer = frameBuffer->GetHandle(),
                .renderArea = renderArea,
                .clearValueCount = static_cast<uint32_t>(clearValues.size()),
                .pClearValues = clearValues.size() == 0 ? nullptr : clearValues.data()
        };
        vkCmdBeginRenderPass(cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void AdVKRenderPass::End(VkCommandBuffer cmdBuffer) const {
        vkCmdEndRenderPass(cmdBuffer);
    }
}