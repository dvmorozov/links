cmake_minimum_required(VERSION 2.8)

project(links)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall -pedantic")
set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(-DLINUX)
add_definitions(-D_UNICODE)

file(GLOB SRC
    "*.h"
    "*.cpp"
    "${PROJECT_SOURCE_DIR}/Controllers/*.h"
    "${PROJECT_SOURCE_DIR}/Controllers/*.cpp"
    "${PROJECT_SOURCE_DIR}/Models/*.h"
    "${PROJECT_SOURCE_DIR}/Models/*.cpp"
    "${PROJECT_SOURCE_DIR}/Utils/*.h"
    "${PROJECT_SOURCE_DIR}/Utils/*.cpp"
    "${PROJECT_SOURCE_DIR}/Views/*.h"
    "${PROJECT_SOURCE_DIR}/Views/*.cpp"
)

list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/Models/ConfigTest.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/Models/ConfigTest.h")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/Models/FileListReaderTest.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/Models/FileListReaderTest.h")
#   Windows sources removed.
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/Models/RegConfig.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/Models/RegConfig.h")

add_executable(${PROJECT_NAME} ${SRC})
