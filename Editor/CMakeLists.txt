cmake_minimum_required(VERSION 3.22)

add_library(am_editor
        Private/AdEditorApp.cpp
        Private/Windows/AdEditorMainWindow.cpp
        Private/Windows/AdEditorViewportWindow.cpp
        Private/Windows/AdEditorContentWindow.cpp
        Private/Windows/AdEditorLayerWindow.cpp
        Private/Windows/AdEditorPropertyWindow.cpp
        Private/Windows/AdEditorSettingWindow.cpp

)
target_link_libraries(am_editor PUBLIC am_core)
