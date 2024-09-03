#include "Render/AdMaterial.h"

namespace ade{
    AdMaterialFactory AdMaterialFactory::s_MaterialFactory{};

    bool AdMaterial::HasTexture(uint32_t id) const {
        if(mTextures.find(id) != mTextures.end()){
            return true;
        }
        return false;
    }

    const TextureView *AdMaterial::GetTextureView(uint32_t id) const {
        if(HasTexture(id)){
            return &mTextures.at(id);
        }
        return nullptr;
    }

    void AdMaterial::SetTextureView(uint32_t id, AdTexture *texture, AdSampler *sampler) {
        if(HasTexture(id)){
            mTextures[id].texture = texture;
            mTextures[id].sampler = sampler;
        } else {
            mTextures[id] = { texture, sampler };
        }
        bShouldFlushResource = true;
    }

    void AdMaterial::UpdateTextureViewEnable(uint32_t id, bool enable) {
        if(HasTexture(id)){
            mTextures[id].bEnable = enable;
            bShouldFlushParams = true;
        }
    }

    void AdMaterial::UpdateTextureViewUVTranslation(uint32_t id, const glm::vec2 &uvTranslation) {
        if(HasTexture(id)){
            mTextures[id].uvTranslation = uvTranslation;
            bShouldFlushParams = true;
        }
    }

    void AdMaterial::UpdateTextureViewUVRotation(uint32_t id, float uvRotation) {
        if(HasTexture(id)){
            mTextures[id].uvRotation = uvRotation;
            bShouldFlushParams = true;
        }
    }

    void AdMaterial::UpdateTextureViewUVScale(uint32_t id, const glm::vec2 &uvScale) {
        if(HasTexture(id)){
            mTextures[id].uvScale = uvScale;
            bShouldFlushParams = true;
        }
    }
}