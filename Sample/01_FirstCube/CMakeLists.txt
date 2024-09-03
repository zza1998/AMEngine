cmake_minimum_required(VERSION 3.22)

add_executable(01_FirstCube
        ${SHADER_HEADERS}
        Main.cpp
)
target_link_libraries(01_FirstCube PRIVATE adiosy_core)