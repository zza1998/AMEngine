#ifndef ADRENDERCONTEXT_H
#define ADRENDERCONTEXT_H

#include "Graphic/AdVKGraphicContext.h"
#include "Graphic/AdVkSwapchain.h"
#include "Graphic/AdVKDevice.h"

namespace ade{
    class AdWindow;

    struct PerformanceParams{
        uint64_t vertexCount = 0;
        uint64_t indexCount = 0;
        uint64_t drawCallCount = 0;
    };
    class AdRenderContext{
    public:
        AdRenderContext(AdWindow *window);
        ~AdRenderContext();

        const PerformanceParams &GetPerformanceParams() const { return mPerformanceParams; }
        void ResetPerformanceParams() { mPerformanceParams.vertexCount = 0; mPerformanceParams.drawCallCount = 0; mPerformanceParams.indexCount = 0; }
        AdGraphicContext *GetGraphicContext() const { return mGraphicContext.get(); }
        AdVKDevice *GetDevice() const { return mDevice.get(); }
        AdVKSwapchain *GetSwapchain() const { return mSwapchain.get(); }
    private:
        std::shared_ptr<AdGraphicContext> mGraphicContext;
        std::shared_ptr<AdVKDevice> mDevice;
        std::shared_ptr<AdVKSwapchain> mSwapchain;
        PerformanceParams mPerformanceParams{};
    };
}

#endif