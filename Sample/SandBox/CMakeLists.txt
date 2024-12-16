cmake_minimum_required(VERSION 3.22)


add_executable(Galaxy
        ${SHADER_HEADERS}
        Main.cpp
        AdEventTesting.cpp
)
target_link_libraries(Galaxy PRIVATE am_editor)