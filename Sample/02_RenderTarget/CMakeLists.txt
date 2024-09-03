cmake_minimum_required(VERSION 3.22)

add_executable(02_RenderTarget
        ${SHADER_HEADERS}
        Main.cpp
)
target_link_libraries(02_RenderTarget PRIVATE adiosy_core)