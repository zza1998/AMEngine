#include "Graphic/AdVKGraphicPipeline.h"
#include "AdFileUtil.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKRenderPass.h"

namespace ade{
    AdVKPipelineLayout::AdVKPipelineLayout(AdVKDevice *device, const std::string &vertexShaderFile, const std::string &fragShaderFile, const ShaderLayout &shaderLayout) : mDevice(device) {
        // compile shaders
        CALL_VK(CreateShaderModule(vertexShaderFile + ".spv", &mVertexShaderModule));
        CALL_VK(CreateShaderModule(fragShaderFile + ".spv", &mFragShaderModule));
        // pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .setLayoutCount = static_cast<uint32_t>(shaderLayout.descriptorSetLayouts.size()),
                .pSetLayouts = shaderLayout.descriptorSetLayouts.data(),
                .pushConstantRangeCount = static_cast<uint32_t>(shaderLayout.pushConstants.size()),
                .pPushConstantRanges = shaderLayout.pushConstants.data()
        };
        CALL_VK(vkCreatePipelineLayout(mDevice->GetHandle(), &pipelineLayoutInfo, nullptr, &mHandle));
    }

    AdVKPipelineLayout::AdVKPipelineLayout(AdVKDevice *device, const std::string &compShaderFile,
        const ShaderLayout &shaderLayout) : mDevice(device){
        // compile shaders
        CALL_VK(CreateShaderModule(compShaderFile + ".spv", &mCompShaderModule));
        // pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = static_cast<uint32_t>(shaderLayout.descriptorSetLayouts.size()),
            .pSetLayouts = shaderLayout.descriptorSetLayouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(shaderLayout.pushConstants.size()),
            .pPushConstantRanges = shaderLayout.pushConstants.data()
        };
        CALL_VK(vkCreatePipelineLayout(mDevice->GetHandle(), &pipelineLayoutInfo, nullptr, &mHandle));
    }

    AdVKPipelineLayout::~AdVKPipelineLayout() {
        VK_D(ShaderModule, mDevice->GetHandle(), mVertexShaderModule);
        VK_D(ShaderModule, mDevice->GetHandle(), mFragShaderModule);
        VK_D(ShaderModule, mDevice->GetHandle(), mCompShaderModule);
        VK_D(PipelineLayout, mDevice->GetHandle(), mHandle);
    }

    VkResult AdVKPipelineLayout::CreateShaderModule(const std::string &filePath, VkShaderModule *outShaderModule) {
        std::vector<char> content = ReadCharArrayFromFile(filePath);

        VkShaderModuleCreateInfo shaderModuleInfo = {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .codeSize = static_cast<uint32_t>(content.size()),
                .pCode = reinterpret_cast<const uint32_t *>(content.data())
        };

        return vkCreateShaderModule(mDevice->GetHandle(), &shaderModuleInfo, nullptr, outShaderModule);
    }

    ////// Pipeline

    AdVKPipeline::AdVKPipeline(AdVKDevice *device, AdVKRenderPass *renderPass, AdVKPipelineLayout *pipelineLayout) : mDevice(device), mRenderPass(renderPass), mPipelineLayout(pipelineLayout) {

    }

    AdVKPipeline::~AdVKPipeline() {
        VK_D(Pipeline, mDevice->GetHandle(), mHandle);
    }

    void AdVKPipeline::Create() {
        VkPipelineShaderStageCreateInfo shaderStageInfo[] = {
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,
                        .stage = VK_SHADER_STAGE_VERTEX_BIT,
                        .module = mPipelineLayout->GetVertexShaderModule(),
                        .pName = "main",
                        .pSpecializationInfo = nullptr
                },
                {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,
                        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                        .module = mPipelineLayout->GetFragShaderModule(),
                        .pName = "main",
                        .pSpecializationInfo = nullptr
                }
        };

        VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .vertexBindingDescriptionCount = static_cast<uint32_t>(mPipelineConfig.vertexInputState.vertexBindings.size()),
                .pVertexBindingDescriptions = mPipelineConfig.vertexInputState.vertexBindings.data(),
                .vertexAttributeDescriptionCount = static_cast<uint32_t>(mPipelineConfig.vertexInputState.vertexAttributes.size()),
                .pVertexAttributeDescriptions = mPipelineConfig.vertexInputState.vertexAttributes.data()
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .topology = mPipelineConfig.inputAssemblyState.topology,
                .primitiveRestartEnable = mPipelineConfig.inputAssemblyState.primitiveRestartEnable
        };

        VkViewport defaultViewport = {
                .x = 0,
                .y = 0,
                .width = 100,
                .height = 100,
                .minDepth = 0,
                .maxDepth = 1
        };
        VkRect2D defaultScissor = {
                .offset = { 0, 0 },
                .extent = { 100, 100 }
        };
        VkPipelineViewportStateCreateInfo viewportStateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .viewportCount = 1,
                .pViewports = &defaultViewport,
                .scissorCount = 1,
                .pScissors = &defaultScissor
        };

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .depthClampEnable = mPipelineConfig.rasterizationState.depthClampEnable,
                .rasterizerDiscardEnable = mPipelineConfig.rasterizationState.rasterizerDiscardEnable,
                .polygonMode = mPipelineConfig.rasterizationState.polygonMode,
                .cullMode = mPipelineConfig.rasterizationState.cullMode,
                .frontFace = mPipelineConfig.rasterizationState.frontFace,
                .depthBiasEnable = mPipelineConfig.rasterizationState.depthBiasEnable,
                .depthBiasConstantFactor = mPipelineConfig.rasterizationState.depthBiasConstantFactor,
                .depthBiasClamp = mPipelineConfig.rasterizationState.depthBiasClamp,
                .depthBiasSlopeFactor = mPipelineConfig.rasterizationState.depthBiasSlopeFactor,
                .lineWidth = mPipelineConfig.rasterizationState.lineWidth
        };

        VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .rasterizationSamples = mPipelineConfig.multisampleState.rasterizationSamples,
                .sampleShadingEnable = mPipelineConfig.multisampleState.sampleShadingEnable,
                .minSampleShading = mPipelineConfig.multisampleState.minSampleShading,
                .pSampleMask = nullptr,
                .alphaToCoverageEnable = VK_FALSE,
                .alphaToOneEnable = VK_FALSE
        };

        VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .depthTestEnable = mPipelineConfig.depthStencilState.depthTestEnable,
                .depthWriteEnable = mPipelineConfig.depthStencilState.depthWriteEnable,
                .depthCompareOp = mPipelineConfig.depthStencilState.depthCompareOp,
                .depthBoundsTestEnable = mPipelineConfig.depthStencilState.depthBoundsTestEnable,
                .stencilTestEnable = mPipelineConfig.depthStencilState.stencilTestEnable,
                .front = {},
                .back = {},
                .minDepthBounds = 0.0f,
                .maxDepthBounds = 0.0f
        };

        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .logicOpEnable = VK_FALSE,
                .logicOp = VK_LOGIC_OP_CLEAR,
                .attachmentCount =  static_cast<uint32_t>(mPipelineConfig.colorBlendAttachmentStateList.size()),
                .pAttachments = mPipelineConfig.colorBlendAttachmentStateList.data(),
        };
        colorBlendStateInfo.blendConstants[0] = colorBlendStateInfo.blendConstants[1] = colorBlendStateInfo.blendConstants[2] = colorBlendStateInfo.blendConstants[3] = 0;

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .dynamicStateCount = static_cast<uint32_t>(mPipelineConfig.dynamicState.dynamicStates.size()),
                .pDynamicStates = mPipelineConfig.dynamicState.dynamicStates.data()
        };
        VkGraphicsPipelineCreateInfo pipelineInfo = {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stageCount = ARRAY_SIZE(shaderStageInfo),
                .pStages = shaderStageInfo,
                .pVertexInputState = &vertexInputStateInfo,
                .pInputAssemblyState = &inputAssemblyStateInfo,
                .pTessellationState = nullptr,
                .pViewportState = &viewportStateInfo,
                .pRasterizationState = &rasterizationStateInfo,
                .pMultisampleState = &multisampleStateInfo,
                .pDepthStencilState = &depthStencilStateInfo,
                .pColorBlendState = &colorBlendStateInfo,
                .pDynamicState = &dynamicStateInfo,
                .layout = mPipelineLayout->GetHandle(),
                .renderPass = mRenderPass->GetHandle(),
                .subpass = mPipelineConfig.subPassNo,
                .basePipelineHandle = VK_NULL_HANDLE,
                .basePipelineIndex = 0
        };
        CALL_VK(vkCreateGraphicsPipelines(mDevice->GetHandle(), mDevice->GetPipelineCache(), 1, &pipelineInfo, nullptr, &mHandle));
        LOG_T("Create pipeline : {0}", (void*)mHandle);
    }

    AdVKPipeline *AdVKPipeline::SetVertexInputState(const std::vector<VkVertexInputBindingDescription> &vertexBindings,
                                                    const std::vector<VkVertexInputAttributeDescription> &vertexAttrs) {
        mPipelineConfig.vertexInputState.vertexBindings = vertexBindings;
        mPipelineConfig.vertexInputState.vertexAttributes = vertexAttrs;
        return this;
    }

    AdVKPipeline *AdVKPipeline::SetInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable) {
        mPipelineConfig.inputAssemblyState.topology = topology;
        mPipelineConfig.inputAssemblyState.primitiveRestartEnable = primitiveRestartEnable;
        return this;
    }

    AdVKPipeline *AdVKPipeline::SetRasterizationState(const PipelineRasterizationState &rasterizationState) {
        mPipelineConfig.rasterizationState.depthClampEnable = rasterizationState.depthClampEnable;
        mPipelineConfig.rasterizationState.rasterizerDiscardEnable = rasterizationState.rasterizerDiscardEnable;
        mPipelineConfig.rasterizationState.polygonMode = rasterizationState.polygonMode;
        mPipelineConfig.rasterizationState.cullMode = rasterizationState.cullMode;
        mPipelineConfig.rasterizationState.frontFace = rasterizationState.frontFace;
        mPipelineConfig.rasterizationState.depthBiasEnable = rasterizationState.depthBiasEnable;
        mPipelineConfig.rasterizationState.depthBiasConstantFactor = rasterizationState.depthBiasConstantFactor;
        mPipelineConfig.rasterizationState.depthBiasClamp = rasterizationState.depthBiasClamp;
        mPipelineConfig.rasterizationState.depthBiasSlopeFactor = rasterizationState.depthBiasSlopeFactor;
        mPipelineConfig.rasterizationState.lineWidth = rasterizationState.lineWidth;
        return this;
    }

    AdVKPipeline *AdVKPipeline::SetMultisampleState(VkSampleCountFlagBits samples, VkBool32 sampleShadingEnable, float minSampleShading) {
        mPipelineConfig.multisampleState.rasterizationSamples = samples;
        mPipelineConfig.multisampleState.sampleShadingEnable = sampleShadingEnable;
        mPipelineConfig.multisampleState.minSampleShading = minSampleShading;
        return this;
    }

    AdVKPipeline *AdVKPipeline::SetDepthStencilState(const PipelineDepthStencilState &depthStencilState) {
        mPipelineConfig.depthStencilState.depthTestEnable = depthStencilState.depthTestEnable;
        mPipelineConfig.depthStencilState.depthWriteEnable = depthStencilState.depthWriteEnable;
        mPipelineConfig.depthStencilState.depthCompareOp = depthStencilState.depthCompareOp;
        mPipelineConfig.depthStencilState.depthBoundsTestEnable = depthStencilState.depthBoundsTestEnable;
        mPipelineConfig.depthStencilState.stencilTestEnable = depthStencilState.stencilTestEnable;
        return this;
    }

    AdVKPipeline *AdVKPipeline::SetColorBlendAttachmentState(VkBool32 blendEnable, VkBlendFactor srcColorBlendFactor,
                                                             VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp,
                                                             VkBlendFactor srcAlphaBlendFactor,
                                                             VkBlendFactor dstAlphaBlendFactor,
                                                             VkBlendOp alphaBlendOp) {
        mPipelineConfig.colorBlendAttachmentStateList.clear();
        mPipelineConfig.colorBlendAttachmentState.blendEnable = blendEnable;
        mPipelineConfig.colorBlendAttachmentState.srcColorBlendFactor = srcColorBlendFactor;
        mPipelineConfig.colorBlendAttachmentState.dstColorBlendFactor = dstColorBlendFactor;
        mPipelineConfig.colorBlendAttachmentState.srcAlphaBlendFactor = srcAlphaBlendFactor;
        mPipelineConfig.colorBlendAttachmentState.dstAlphaBlendFactor = dstAlphaBlendFactor;
        mPipelineConfig.colorBlendAttachmentState.alphaBlendOp = alphaBlendOp;

        mPipelineConfig.colorBlendAttachmentStateList.push_back(mPipelineConfig.colorBlendAttachmentState);
        return this;
    }

    AdVKPipeline *AdVKPipeline::SetColorBlendAttachmentStateDefault(uint32_t attachmentNum) {
        mPipelineConfig.colorBlendAttachmentStateList.clear();
        for(int i =0;i<attachmentNum;i++) {
            mPipelineConfig.colorBlendAttachmentStateList.push_back(mPipelineConfig.colorBlendAttachmentState);
        }
        return this;
    }

    AdVKPipeline *AdVKPipeline::SetDynamicState(const std::vector<VkDynamicState> &dynamicStates) {
        mPipelineConfig.dynamicState.dynamicStates = dynamicStates;
        return this;
    }

    AdVKPipeline *AdVKPipeline::EnableAlphaBlend() {
        mPipelineConfig.colorBlendAttachmentState = {
                .blendEnable = VK_TRUE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD
        };
        return this;
    }

    AdVKPipeline *AdVKPipeline::EnableDepthTest() {
        mPipelineConfig.depthStencilState.depthTestEnable = VK_TRUE;
        mPipelineConfig.depthStencilState.depthWriteEnable = VK_TRUE;
        mPipelineConfig.depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
        return this;
    }

    AdVKPipeline *AdVKPipeline::SetCullingMode(VkCullModeFlagBits cullMode) {
        mPipelineConfig.rasterizationState.cullMode = cullMode;
        return this;
    }
    AdVKPipeline *AdVKPipeline::DisableDepthWriteButTest() {
        mPipelineConfig.depthStencilState.depthTestEnable = VK_TRUE;
        mPipelineConfig.depthStencilState.depthWriteEnable = VK_FALSE;
        mPipelineConfig.depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        return this;
    }
    AdVKPipeline *AdVKPipeline::DisableDepthTestAndWrite() {
        mPipelineConfig.depthStencilState.depthTestEnable = VK_FALSE;
        mPipelineConfig.depthStencilState.depthWriteEnable = VK_FALSE;
        return this;
    }

    AdVKPipeline * AdVKPipeline::SetSubPassNo(uint32_t no) {
        mPipelineConfig.subPassNo = no;
        return this;
    }

    void AdVKPipeline::Bind(VkCommandBuffer cmdBuffer) {
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mHandle);
    }
}