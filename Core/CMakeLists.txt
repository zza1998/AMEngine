cmake_minimum_required(VERSION 3.22)

add_library(am_core
        Private/AdApplication.cpp
        Private/Render/AdRenderContext.cpp
        Private/Render/AdRenderTarget.cpp
        Private/Render/AdRenderer.cpp
        Private/Render/AdMesh.cpp
        Private/Render/AdTexture.cpp
        Private/Render/AdSampler.cpp
        Private/Render/AdMaterial.cpp

        Private/ECS/AdUUID.cpp
        Private/ECS/AdNode.cpp
        Private/ECS/AdScene.cpp
        Private/ECS/AdEntity.cpp
        Private/ECS/Component/AdLookAtCameraComponent.cpp
        Private/ECS/Component/AdTransformComponent.cpp
        Private/ECS/System/AdMaterialSystem.cpp
        Private/ECS/System/AdBaseMaterialSystem.cpp
        Private/ECS/System/AdUnlitMaterialSystem.cpp
        Private/ECS/System/AdPhongMaterialSystem.cpp
        Private/ECS/System/AdPBRMaterialSystem.cpp

        Private/Loader/ModelLoader.cpp

        Private/Gui/Widget/AdFileBrowserWidget.cpp
        Private/Gui/Widget/AdTransformWidget.cpp
        Private/Gui/Widget/AdWidget.cpp

)
target_link_libraries(am_core PUBLIC am_platform)

if(WIN32)
    target_compile_definitions(am_platform PUBLIC NOMINMAX)
endif()
