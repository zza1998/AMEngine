cmake_minimum_required(VERSION 3.22)

add_executable(04_ECS_Entity
        ${SHADER_HEADERS}
        Main.cpp
)
target_link_libraries(04_ECS_Entity PRIVATE adiosy_core)