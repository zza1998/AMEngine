cmake_minimum_required(VERSION 3.22)

# shader files
set(SHADERS
        00_hello_triangle.vert
        00_hello_triangle.frag
        01_hello_buffer.vert
        01_hello_buffer.frag
        02_descriptor_set.frag
        02_descriptor_set.vert
        03_unlit_material.frag
        03_unlit_material.vert
        04_phong_material.frag
        04_phong_material.vert
)
spirv_shaders(
        SHADER_HEADERS
        SPIRV_VERSION
        1.0 # Currently targeting Vulkan 1.0
        SOURCES
        ${SHADERS}
)

add_executable(SandBox
        ${SHADER_HEADERS}
        Main.cpp
        AdEventTesting.cpp
)
target_link_libraries(SandBox PRIVATE adiosy_core)