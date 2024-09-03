#ifndef ADBASERENDERCOMPONENT_H
#define ADBASERENDERCOMPONENT_H

#include "AdMaterialComponent.h"

namespace ade{
    enum BaseMaterialColor{
        COLOR_TYPE_NORMAL = 0,
        COLOR_TYPE_TEXCOORD = 1
    };

    struct AdBaseMaterial : public AdMaterial{
        BaseMaterialColor colorType;
    };

    struct AdBaseMaterialComponent : public AdMaterialComponent<AdBaseMaterial> {

    };
}

#endif