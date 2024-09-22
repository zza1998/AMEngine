#ifndef ADTEXTURE_H
#define ADTEXTURE_H

#include "Graphic/AdVKCommon.h"
#include "AdSampler.h"
#include "ktx/include/ktx.h"
namespace ade{
    class AdVKImage;
    class AdVKImageView;
    class AdVKBuffer;

    struct RGBAColor{
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    class AdCubeTexture {
    public:
         AdCubeTexture(const std::string& filename, VkFormat imageFormat);
        ~AdCubeTexture();

        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mMipLevel;
        VkFormat mFormat;
        std::shared_ptr<AdVKImage> mImage;
        std::shared_ptr<AdVKImageView> mImageView;
        std::shared_ptr<AdSampler> mSampler;
    private:
        void CreateImage(size_t size, void *data);
        ktxResult loadKTXFile(std::string filename, ktxTexture **target);
    };

    class AdTexture{
    public:
        // creat texture from pure color or file img
        AdTexture(const std::string &filePath);
        AdTexture(uint32_t width, uint32_t height, RGBAColor *pixels);

        ~AdTexture();

        uint32_t GetWidth() const { return mWidth; }
        uint32_t GetHeight() const { return mHeight; }
        AdVKImage *GetImage() const { return mImage.get(); }
        AdVKImageView *GetImageView() const { return mImageView.get(); }
    private:
        void CreateImage(size_t size, void *data);

        uint32_t mWidth;
        uint32_t mHeight;
        VkFormat mFormat;
        std::shared_ptr<AdVKImage> mImage;
        std::shared_ptr<AdVKImageView> mImageView;
    };
}

#endif