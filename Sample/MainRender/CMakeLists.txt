cmake_minimum_required(VERSION 3.22)

# search all vert and frag
file(GLOB SHADER_PATHS
        "${AD_DEFINE_RES_ROOT_DIR}Shader/*.vert"
        "${AD_DEFINE_RES_ROOT_DIR}Shader/*.frag")
set(SHADER_FILE_NAMES)

# get pure name
foreach(SHADER ${SHADER_PATHS})
    get_filename_component(SHADER_FILE_NAME ${SHADER} NAME)
    list(APPEND SHADER_FILE_NAMES ${SHADER_FILE_NAME})
endforeach()

# log files
# message(STATUS "shaders:${SHADER_FILE_NAMES}" )

spirv_shaders(
        SHADER_HEADERS
        SPIRV_VERSION
        1.0 # Currently targeting Vulkan 1.0
        SOURCES
        ${SHADER_FILE_NAMES}
)

add_executable(MainRender
        ${SHADER_HEADERS}
        Main.cpp
)

target_link_libraries(MainRender PRIVATE am_editor)