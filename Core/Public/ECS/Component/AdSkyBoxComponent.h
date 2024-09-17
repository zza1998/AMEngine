//
// Created by zhouzian on 2024/9/16.
//

#ifndef ADSKYLIGHTCOMPONENT_H
#define ADSKYLIGHTCOMPONENT_H

#include "ECS/AdComponent.h"
namespace ade{

class AdSkyBoxComponent : public AdComponent{


    private:
      std::shared_ptr<AdMesh> mSkyMesh;
};
}
#endif //ADSKYLIGHTCOMPONENT_H
