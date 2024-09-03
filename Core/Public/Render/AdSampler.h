#ifndef ADSAMPLER_H
#define ADSAMPLER_H

#include "Graphic/AdVKCommon.h"

namespace ade{
    class AdSampler{
    public:
        AdSampler(VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
        ~AdSampler();

        VkSampler GetHandle() const { return mHandle; }
    private:
        VkSampler mHandle = VK_NULL_HANDLE;

        VkFilter mFilter;
        VkSamplerAddressMode mAddressMode;
    };
}

#endif