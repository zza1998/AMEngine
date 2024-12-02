cmake_minimum_required(VERSION 3.22)

add_library(am_platform
        Private/AdLog.cpp
        Private/AdGeometryUtil.cpp
        Private/AdWindow.cpp
        Private/Window/AdGLFWwindow.cpp

        Private/AdGraphicContext.cpp
        Private/Graphic/AdVKGraphicContext.cpp
        Private/Graphic/AdVKDevice.cpp
        Private/Graphic/AdVKQueue.cpp
        Private/Graphic/AdVkSwapchain.cpp
        Private/Graphic/AdVKRenderPass.cpp
        Private/Graphic/AdVKFrameBuffer.cpp
        Private/Graphic/AdVKImage.cpp
        Private/Graphic/AdVKImageView.cpp
        Private/Graphic/AdVKPipeline.cpp
        Private/Graphic/AdVKCommandBuffer.cpp
        Private/Graphic/AdVKBuffer.cpp
        Private/Graphic/AdVKDescriptorSet.cpp

        Private/Event/AdEventDispatcher.cpp
)

target_include_directories(am_platform PUBLIC External)

# glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(External/glfw)
target_link_libraries(am_platform PRIVATE glfw)

# glm
add_subdirectory(External/glm)
target_link_libraries(am_platform PRIVATE glm)

#ktx
set(KTX_DIR External/ktx)
set(KTX_SOURCES
        ${KTX_DIR}/include/ktx.h
        ${KTX_DIR}/include/ktxvulkan.h
        ${KTX_DIR}/lib/texture.c
        ${KTX_DIR}/lib/hashlist.c
        ${KTX_DIR}/lib/checkheader.c
        ${KTX_DIR}/lib/swap.c
        ${KTX_DIR}/lib/memstream.c
        ${KTX_DIR}/lib/filestream.c
)
add_library(ktx STATIC ${KTX_SOURCES})
target_include_directories(ktx PUBLIC External/ktx/include)
target_include_directories(ktx PUBLIC External/ktx/other_include)
target_link_libraries(am_platform PRIVATE ktx)


find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message("----> Find vulkan success : ${Vulkan_INCLUDE_DIRS}")
endif()
target_include_directories(am_platform PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(am_platform PRIVATE ${Vulkan_LIBRARY})

# imgui
add_library(imgui
        External/imgui/imconfig.h
        External/imgui/imgui.cpp
        External/imgui/imgui.h
        External/imgui/imgui_demo.cpp
        External/imgui/imgui_draw.cpp
        External/imgui/imgui_impl_glfw.cpp
        External/imgui/imgui_impl_glfw.h
        External/imgui/imgui_impl_vulkan.cpp
        External/imgui/imgui_impl_vulkan.h
        External/imgui/imgui_internal.h
        External/imgui/imgui_tables.cpp
        External/imgui/imgui_widgets.cpp
        External/imgui/imstb_rectpack.h
        External/imgui/imstb_textedit.h
        External/imgui/imstb_truetype.h

        External/imgui/implot.cpp
        External/imgui/implot.h
        External/imgui/implot_demo.cpp
        External/imgui/implot_internal.h
        External/imgui/implot_items.cpp

        External/imgui/ImGuizmo.cpp
        External/imgui/ImGuizmo.h
)
target_include_directories(imgui PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(imgui PRIVATE ${Vulkan_LIBRARY} glfw)
target_link_libraries(am_platform PRIVATE imgui)


target_include_directories(am_platform PUBLIC External/assimp/include)
if(WIN32)
    set(ASSIMP_LIB_DIR "Win64/libassimp-5.dll")
    #set(ASSIMP_LIB_DIR "Win64/assimp-vc143-mt.lib")
elseif(UNIX)
    if(APPLE)
        set(ASSIMP_LIB_DIR "Macos/libassimp.5.dylib")
    else()
        set(ASSIMP_LIB_DIR "Linux/libassimp.so.5")
    endif()
else()
    message("Platform: Unsupported")
endif()
target_link_libraries(am_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/assimp/lib/${ASSIMP_LIB_DIR})

