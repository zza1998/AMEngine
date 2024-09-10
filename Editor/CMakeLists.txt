cmake_minimum_required(VERSION 3.22)

add_library(adiosy_editor
        Private/AdEditorApp.cpp
        Private/Windows/AdEditorMainWindow.cpp
        Private/Windows/AdEditorViewportWindow.cpp
        Private/Windows/AdEditorContentWindow.cpp
        Private/Windows/AdEditorLayerWindow.cpp
        Private/Windows/AdEditorPropertyWindow.cpp

)
target_link_libraries(adiosy_editor PUBLIC adiosy_core)
