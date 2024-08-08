
# Add source to this project's executable.
add_executable(amengine_editor 
    main.cpp
    vk_engine.cpp
    vk_engine.h
    vk_images.h
    vk_pipelines.h
    vk_pipelines.cpp
    vk_types.h
    camera.cpp
    camera.h
    vk_descriptors.h
    vk_descriptors.cpp
    vk_images.cpp
    vk_loader.h
    vk_loader.cpp
    vk_initializers.cpp
    vk_initializers.h
    am_ui.h
    am_ui.cpp
)


set_property(TARGET amengine_editor  PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:amengine_editor>")

# 配置活动项目
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT amengine_editor)

target_include_directories(amengine_editor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(amengine_editor  vkbootstrap vma glm tinyobjloader imgui stb_image fastgltf)

target_link_libraries(amengine_editor  Vulkan::Vulkan sdl2)

add_dependencies(amengine_editor  Shaders)
