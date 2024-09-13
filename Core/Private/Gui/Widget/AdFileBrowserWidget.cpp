#include "Gui/Widget/AdFileBrowserWidget.h"
#include "AdFileUtil.h"
#include "Gui/AdFontAwesomeIcons.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace ade{
    static bool comparePaths(const std::filesystem::path& path1, const std::filesystem::path& path2) {
        return path1.string() < path2.string();
    }

    AdFileBrowserWidget::AdFileBrowserWidget() {
        mCurrentDirectory = AD_RES_ROOT_DIR;
        mBaseDirectory = AD_RES_ROOT_DIR;
    }

    AdFileBrowserWidget::AdFileBrowserWidget(const std::string &dir) {
        if(!dir.empty()){
            mCurrentDirectory = dir;
        } else {
            mCurrentDirectory = AD_RES_ROOT_DIR;
        }
        mBaseDirectory = AD_RES_ROOT_DIR;
    }

    void AdFileBrowserWidget::Draw(const std::function<void(const std::filesystem::path &)> &doubleClickFileCallback) {
        mDoubleClickFileCallback = doubleClickFileCallback;
        // Toolbar
        ImGui::BeginGroup();
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

        bool isBaseDIR = mCurrentDirectory <= std::filesystem::path(mBaseDirectory);
        ImGui::BeginDisabled(isBaseDIR);
        if (ImGui::Button(ade::icon::ArrowLeft, { 30.f, 0.f })){
            mCurrentDirectory = mCurrentDirectory.parent_path();
        }
        ImGui::PopItemWidth();
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::PushID(this + 0);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
        static char dirPath[512];
        memcpy(dirPath, mCurrentDirectory.string().data(), mCurrentDirectory.string().size());
        ImGuiInputTextFlags dirPathInputFlags = ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CtrlEnterForNewLine;
        ImGui::InputText("##value", dirPath, 512, dirPathInputFlags);
        if(ImGui::IsKeyPressed(ImGuiKey_Enter, false)){
            std::filesystem::path newPath = std::filesystem::path(std::string(dirPath));
            if(newPath <= mBaseDirectory){
                newPath = mBaseDirectory;
            }
            mCurrentDirectory = newPath;
        }
        ImGui::PopID();

        ImGui::SameLine();
        ImGui::PushID(this + 1);
        ImGuiTextFilter filter;
        filter.Draw("##value");
        ImGui::PopID();

        ImGui::SameLine();
        if (ImGui::Button(bHorizontalOrder ? ade::icon::ListUl : ade::icon::Th, { 30.f, 0.f })){
            bHorizontalOrder = !bHorizontalOrder;
        }
        ImGui::EndGroup();

        ImGui::Separator();
        std::filesystem::path rootPath(mCurrentDirectory);
        if(!is_directory(rootPath)){
            ImGui::Text("Folder is not exists.");
            return;
        }
        std::vector<std::filesystem::path> dirs;
        std::vector<std::filesystem::path> files;
        for (const auto &entry: std::filesystem::directory_iterator(rootPath)){
            if(!filter.PassFilter(entry.path().filename().string().c_str())){
                continue;
            }
            if(entry.is_directory()){
                dirs.push_back(entry.path());
            } else {
                files.push_back(entry.path());
            }
        }
        if(dirs.empty() && files.empty()){
            ImGui::Text("Folder is empty.");
            return;
        }

        std::sort(dirs.begin(), dirs.end(), comparePaths);
        std::sort(files.begin(), files.end(), comparePaths);
        if(bHorizontalOrder){
            HorizontalOrder(dirs, files);
        } else {
            VerticalOrder(dirs, files);
        }
    }

    void AdFileBrowserWidget::HorizontalOrder(const std::vector<std::filesystem::path> &dirs, const std::vector<std::filesystem::path> &files) {
        static float padding = 16.0f;
        float cellSize = mWidth + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) {
            columnCount = 1;
        }

        ImGui::Columns(columnCount, 0, false);
        for (int n = 0; n < dirs.size(); n++){
            ImGui::PushID(dirs[n].c_str());
            HandleDirOrFile(false, false, dirs[n]);
            ImGui::PopID();
            ImGui::NextColumn();
        }

        for (int n = 0; n < files.size(); n++){
            ImGui::PushID(files[n].c_str());
            HandleDirOrFile(true, false, files[n]);
            ImGui::PopID();
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }

    void AdFileBrowserWidget::VerticalOrder(const std::vector<std::filesystem::path> &dirs, const std::vector<std::filesystem::path> &files) {
        if(ImGui::BeginTable("Files", 3)){
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Name");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Size");
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("Date");

            for (const auto &dir: dirs){
                ImGui::PushID(dir.c_str());
                ImGui::TableNextRow(0, 20.f);

                ImGui::TableSetColumnIndex(0);
                HandleDirOrFile(false, true, dir);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d Item", 999);            // TODO
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", FormatSystemTime(last_write_time(dir)).c_str());

                ImGui::PopID();
            }

            for (const auto &file: files){
                ImGui::PushID(file.c_str());
                ImGui::TableNextRow(0, 20.f);

                ImGui::TableSetColumnIndex(0);
                HandleDirOrFile(true, true, file);

                ImGui::TableSetColumnIndex(1);
                float size;
                std::string unit;
                FormatFileSize(file_size(file), &size, unit);
                ImGui::Text("%.2f %s", size, unit.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", FormatSystemTime(last_write_time(file)).c_str());
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    void AdFileBrowserWidget::HandleDirOrFile(bool isFile, bool verticalOrder, const std::filesystem::path &path) {
        ImFontAtlas *fonts = ImGui::GetIO().Fonts;
        std::string icon = ade::icon::File;

        if (mSelectedFilePath == path) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(verticalOrder ? ImGuiCol_WindowBg : ImGuiCol_FrameBg));
        }

        if(verticalOrder){
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0, 2 });
            ImGui::ButtonEx((icon + "  " + path.filename().string()).c_str(), { 1000.f, 0.f });
            ImGui::PopStyleVar();
        } else {
            ImGui::PushFont(fonts->Fonts[fonts->Fonts.Size - 1]);
            ImGui::Button(icon.c_str(), { mWidth, mHeight });
            ImGui::PopFont();
        }
        ImGui::PopStyleColor();

        if(isFile){
            if (ImGui::BeginDragDropSource()){
                ImGui::Text("%s", path.filename().c_str());
                const wchar_t* itemPath = reinterpret_cast<const wchar_t *>(path.c_str());
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                ImGui::EndDragDropSource();
            }
        }

        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                mSelectedFilePath = path;
            }
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if(isFile){
                    mDoubleClickFileCallback(mSelectedFilePath);
                } else {
                    mCurrentDirectory /= path.filename();
                }
            }
        }

        if(!verticalOrder){
            ImGui::Text(" ");
            ImGui::SameLine(ImGui::GetFrameHeight());
            ImGui::TextWrapped("%s", path.filename().c_str());
        }
    }
}