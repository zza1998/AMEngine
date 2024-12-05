#ifndef AD_VKPIPELINE_H
#define AD_VKPIPELINE_H

#include "AdVKCommon.h"

namespace ade{
    class AdVKDevice;
    class AdVKRenderPass;

    struct ShaderLayout{
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        std::vector<VkPushConstantRange> pushConstants;
    };

    struct PipelineVertexInputState{
        std::vector<VkVertexInputBindingDescription> vertexBindings;
        std::vector<VkVertexInputAttributeDescription> vertexAttributes;
    };

    struct PipelineInputAssemblyState{
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkBool32 primitiveRestartEnable = VK_FALSE;
    };

    struct PipelineRasterizationState{
        VkBool32 depthClampEnable = VK_FALSE;
        VkBool32 rasterizerDiscardEnable = VK_FALSE;
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
        VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
        VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        VkBool32 depthBiasEnable = VK_FALSE;
        float depthBiasConstantFactor = 0;
        float depthBiasClamp = 0;
        float depthBiasSlopeFactor = 0;
        float lineWidth = 1.f;
    };

    struct PipelineMultisampleState{
        VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        VkBool32 sampleShadingEnable = VK_FALSE;
        float minSampleShading = 0.2f;
    };

    struct PipelineDepthStencilState{
        VkBool32 depthTestEnable = VK_FALSE;
        VkBool32 depthWriteEnable = VK_FALSE;
        VkCompareOp depthCompareOp = VK_COMPARE_OP_NEVER;
        VkBool32 depthBoundsTestEnable = VK_FALSE;
        VkBool32 stencilTestEnable = VK_FALSE;
    };

    struct PipelineDynamicState{
        std::vector<VkDynamicState> dynamicStates;
    };

    struct PipelineConfig{
        PipelineVertexInputState vertexInputState;
        PipelineInputAssemblyState inputAssemblyState;
        PipelineRasterizationState rasterizationState;
        PipelineMultisampleState multisampleState;
        PipelineDepthStencilState depthStencilState;
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
                .blendEnable = VK_FALSE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                  | VK_COLOR_COMPONENT_G_BIT
                                  | VK_COLOR_COMPONENT_B_BIT
                                  | VK_COLOR_COMPONENT_A_BIT
        };
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStateList{colorBlendAttachmentState};
        PipelineDynamicState dynamicState;
        uint32_t subPassNo = 0 ;
    };

    class AdVKPipelineLayout{
    public:
        AdVKPipelineLayout(AdVKDevice *device, const std::string &vertexShaderFile, const std::string &fragShaderFile, const ShaderLayout &shaderLayout = {});
        AdVKPipelineLayout(AdVKDevice *device, const std::string &compShaderFile, const ShaderLayout &shaderLayout = {});
        ~AdVKPipelineLayout();

        VkPipelineLayout GetHandle() const { return mHandle; }
        VkShaderModule GetVertexShaderModule() const { return mVertexShaderModule; }
        VkShaderModule GetFragShaderModule() const { return mFragShaderModule; }
        VkShaderModule GetCompShaderModule() const { return mCompShaderModule; }
    private:
        VkResult CreateShaderModule(const std::string &filePath, VkShaderModule *outShaderModule);

        VkPipelineLayout mHandle = VK_NULL_HANDLE;

        VkShaderModule mVertexShaderModule = VK_NULL_HANDLE;
        VkShaderModule mFragShaderModule = VK_NULL_HANDLE;
        VkShaderModule mCompShaderModule = VK_NULL_HANDLE;
        AdVKDevice *mDevice;
    };

    class AdVKPipeline{
    public:
        AdVKPipeline(AdVKDevice *device, AdVKRenderPass *renderPass, AdVKPipelineLayout *pipelineLayout);
        ~AdVKPipeline();
        void Create();

        void Bind(VkCommandBuffer cmdBuffer);

        AdVKPipeline *SetVertexInputState(const std::vector<VkVertexInputBindingDescription> &vertexBindings, const std::vector<VkVertexInputAttributeDescription> &vertexAttrs);
        AdVKPipeline *SetInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable = VK_FALSE);
        AdVKPipeline *SetRasterizationState(const PipelineRasterizationState &rasterizationState);
        AdVKPipeline *SetMultisampleState(VkSampleCountFlagBits samples, VkBool32 sampleShadingEnable, float minSampleShading = 0.f);
        AdVKPipeline *SetDepthStencilState(const PipelineDepthStencilState &depthStencilState);
        AdVKPipeline *SetColorBlendAttachmentState(VkBool32 blendEnable,
                                                   VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
                                                   VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD);
        AdVKPipeline *SetDynamicState(const std::vector<VkDynamicState> &dynamicStates);
        AdVKPipeline *SetColorBlendAttachmentStateDefault(uint32_t attachmentNum);
        AdVKPipeline *EnableAlphaBlend();
        AdVKPipeline *EnableDepthTest();
        AdVKPipeline *DisableDepthWriteButTest();
        AdVKPipeline *DisableDepthTestAndWrite();
        AdVKPipeline *SetSubPassNo(uint32_t no);
        AdVKPipeline* SetCullingMode(VkCullModeFlagBits cullMode);
        VkPipeline GetHandle() const { return mHandle; }
    private:
        VkPipeline mHandle = VK_NULL_HANDLE;
        AdVKDevice *mDevice;
        AdVKRenderPass *mRenderPass;
        AdVKPipelineLayout *mPipelineLayout;

        PipelineConfig mPipelineConfig;
    };
}

#endif