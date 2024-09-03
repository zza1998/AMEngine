#include "Render/AdRenderContext.h"

namespace ade{
    AdRenderContext::AdRenderContext(AdWindow *window) {
        mGraphicContext = ade::AdGraphicContext::Create(window);
        auto vkContext = dynamic_cast<ade::AdVKGraphicContext*>(mGraphicContext.get());
        mDevice = std::make_shared<ade::AdVKDevice>(vkContext, 1, 1);
        mSwapchain = std::make_shared<ade::AdVKSwapchain>(vkContext, mDevice.get());
    }

    AdRenderContext::~AdRenderContext() {

    }
}