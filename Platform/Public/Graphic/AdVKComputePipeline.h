//
// Created by zhou on 2024/12/3.
//

#ifndef ADVKCOMPUTEPIPELINE_H
#define ADVKCOMPUTEPIPELINE_H

#endif //ADVKCOMPUTEPIPELINE_H

namespace ade {

    class AdVkComputePipeline {

    public:
        AdVkComputePipeline(VkShaderModule shaderModule,VkPipelineLayout layout,VkDevice device);
        ~AdVkComputePipeline();

        VkPipeline Get() {return pipeline;}
    private:
        VkPipeline pipeline{};
        VkDevice mDevice;
    };

}
