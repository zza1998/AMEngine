cmake_minimum_required(VERSION 3.22)

# shader files


add_executable(SandBox
        ${SHADER_HEADERS}
        Main.cpp
        AdEventTesting.cpp
)
target_link_libraries(SandBox PRIVATE am_core)