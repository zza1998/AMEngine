#ifndef ADAPPLICATIONCONTEXT_H
#define ADAPPLICATIONCONTEXT_H

#include "ECS/AdScene.h"

namespace ade{
    class AdApplication;
    class AdRenderContext;

    struct AdAppContext{
        AdApplication *app;
        AdScene *scene;
        AdRenderContext *renderCxt;
    };
}

#endif