#ifndef ADFILEBROWSERLAYER_H
#define ADFILEBROWSERLAYER_H

#include "AdEngine.h"

namespace ade{
    class AdFileBrowserWidget{
    public:
        AdFileBrowserWidget();
        AdFileBrowserWidget(const std::string &dir);

        void Draw(const std::function<void(const std::filesystem::path&)> &doubleClickFileCallback = {});
        void SetDefaultFilterSuffix(const std::string &filterText){ mDefaultFilterSuffix = filterText; }
        const std::filesystem::path& GetSelectedFilePath() const { return mSelectedFilePath; };
    private:
        void HorizontalOrder(const std::vector<std::filesystem::path> &dirs, const std::vector<std::filesystem::path> &files);
        void VerticalOrder(const std::vector<std::filesystem::path> &dirs, const std::vector<std::filesystem::path> &files);
        void HandleDirOrFile(bool isFile, bool verticalOrder, const std::filesystem::path &path);

        std::filesystem::path mBaseDirectory;
        std::filesystem::path mCurrentDirectory;
        std::filesystem::path mSelectedFilePath;
        std::string mDefaultFilterSuffix;
        std::function<void(const std::filesystem::path&)> mDoubleClickFileCallback;

        bool bHorizontalOrder = true;
        float mWidth = 48.f;
        float mHeight = 48.f;
    };
}
#endif