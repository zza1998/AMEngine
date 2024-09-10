cmake_minimum_required(VERSION 3.22)

project(AMEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -Wall")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#resource dir configuration
add_definitions(-DAD_DEFINE_RES_ROOT_DIR=\"${CMAKE_SOURCE_DIR}/Resource/\")
set(AD_DEFINE_RES_ROOT_DIR "${CMAKE_SOURCE_DIR}/Resource/")

if(WIN32)
    message("Platform: Windows")
    add_definitions(-DAD_ENGINE_PLATFORM_WIN32)
elseif(UNIX)
    if(APPLE)
        message("Platform: Macos")
        add_definitions(-DAD_ENGINE_PLATFORM_MACOS)
    else()
        message("Platform: Linux")
        add_definitions(-DAD_ENGINE_PLATFORM_LINUX)
    endif()
else()
    message("Platform: Unsupported")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SPIR-V)

include_directories(Platform/Public)
include_directories(Editor/Public)
include_directories(Core/Public)

add_subdirectory(Platform)
add_subdirectory(Core)
add_subdirectory(Editor)
add_subdirectory(Sample)