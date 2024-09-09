cmake_minimum_required(VERSION 3.22)

add_library(adiosy_editor
        Private/AdEditorApp.cpp
        Private/Window/AdEditorMainWindow.cpp
        Private/Window/AdEditorViewportWindow.cpp
        Private/Window/AdEditorContentWindow.cpp
        Private/Window/AdEditorLayerWindow.cpp
        Private/Window/AdEditorPropertyWindow.cpp
)
target_link_libraries(adiosy_editor PUBLIC adiosy_core)
