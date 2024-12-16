//
// Created by zhou on 2024/12/3.
//
#include "AdFileUtil.h"
#include "Graphic/AdVKDevice.h"
#include "Graphic/AdVKRenderPass.h"
#include "Graphic/AdVKComputePipeline.h"

namespace ade {
    AdVkComputePipeline::AdVkComputePipeline(VkShaderModule shaderModule, VkPipelineLayout layout, VkDevice device):mDevice(device) {

        // 创建计算管线
        // 创建计算着色器阶段信息
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStageCreateInfo.module = shaderModule; // 计算着色器模块
        shaderStageCreateInfo.pName = "main"; // 着色器入口点名称

        // 创建计算管线创建信息
        VkComputePipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stage = shaderStageCreateInfo; // 着色器阶段信息
        pipelineCreateInfo.layout = layout; // 管线布局
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // 不使用基础管线
        pipelineCreateInfo.basePipelineIndex = -1;

        // 创建管线

        VkResult result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);

        // 检查管线创建是否成功
        if (result != VK_SUCCESS) {
            // 处理错误
            LOG_E("Failed to create compute pipeline: {0}\n", result);
            exit(1);
        }
    }

    AdVkComputePipeline::~AdVkComputePipeline() {
        VK_D(Pipeline,mDevice,pipeline);
    }
}
