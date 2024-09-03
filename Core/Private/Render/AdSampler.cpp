#include "Render/AdSampler.h"

#include "AdApplication.h"
#include "Render/AdRenderContext.h"

namespace ade{
    AdSampler::AdSampler(VkFilter filter, VkSamplerAddressMode addressMode) : mFilter(filter), mAddressMode(addressMode) {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();

        CALL_VK(device->CreateSimpleSampler(mFilter, mAddressMode, &mHandle));
    }

    AdSampler::~AdSampler() {
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();
        VK_D(Sampler, device->GetHandle(), mHandle);
    }
}