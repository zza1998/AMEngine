//
// Created by zhouzian on 2024/9/16.
//

#ifndef ADSKYLIGHTCOMPONENT_H
#define ADSKYLIGHTCOMPONENT_H

#include "ECS/AdComponent.h"
#include "Render/AdMesh.h"
namespace ade{

class AdSkyBoxComponent : public AdComponent{
public:
    std::shared_ptr<AdMesh > GetSkyBoxCube() const {
        return mSkyBoxCube;
    }

    void SetSkyBoxCube(const std::shared_ptr<AdMesh > &cube) {
        mSkyBoxCube = cube;
    }

    std::shared_ptr<AdCubeTexture> GetTexture() const {
        return mTexture;
    }

    void SetTexture(const std::shared_ptr<AdCubeTexture> &m_texture) {
        mTexture = m_texture;
    }

    std::shared_ptr<AdSampler> GetSampler() const {
        return mSampler;
    }
    void SetSampler(const std::shared_ptr<AdSampler> &sampler) {}

private:
    std::shared_ptr<AdMesh> mSkyBoxCube;
    std::shared_ptr<AdCubeTexture> mTexture;
    std::shared_ptr<AdSampler> mSampler;
};
}
#endif //ADSKYLIGHTCOMPONENT_H
