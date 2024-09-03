#ifndef ADCOMPONENT_H
#define ADCOMPONENT_H

#include "AdEntity.h"

namespace ade{
    class AdComponent{
    public:
        void SetOwner(AdEntity *owner){ mOwner = owner; }
        AdEntity *GetOwner() const { return mOwner; }
    private:
        AdEntity *mOwner = nullptr;
    };
}

#endif