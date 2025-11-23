cmake_minimum_required(VERSION 3.12)

project(links-legacy.exe)

# By default clang++ is used.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -Wall -pedantic" CACHE STRING "")
set(CMAKE_VERBOSE_MAKEFILE ON)

# Check if compiler is GCC version less than 9 or Clang and adjust flags accordingly
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    message(STATUS "GCC version less than 9.0 detected. Adding filesystem library.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    message(STATUS "Clang version less than 9.0 detected. Adding filesystem library.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lc++fs")
endif()

add_definitions(-DLINUX)
add_definitions(-D_UNICODE)

# Debug/Release configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
    message(STATUS "Debug build: _DEBUG defined, debug output enabled")
else()
    message(STATUS "Release build: _DEBUG not defined, debug output disabled")
endif()

file(GLOB SRC
    "${PROJECT_SOURCE_DIR}/../../Sources/App/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Shared/Controllers/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Shared/Models/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Shared/Repositories/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Shared/Services/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Shared/UI/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Features/BookmarkManagement/Models/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Features/BookmarkManagement/Views/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Features/AIAutoOrganization/Models/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Features/AIAutoOrganization/Views/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Features/Authentication/Views/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Features/UI/Core/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Models/TestSpecific/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Models/WindowsSpecific/*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Sources/Utils/*.cpp"
)

list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/ConfigTest.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/ConfigTest.h")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/FileListReaderTest.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/FileListReaderTest.h")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/TestSpecific/TextFileReaderTest.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/TestSpecific/TextFileReaderTest.h")
#   Windows sources removed.
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/RegConfig.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/RegConfig.h")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/WindowsSpecific/ConfigFactory.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/WindowsSpecific/ConfigFactory.h")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/WindowsSpecific/RegConfig.cpp")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/../../Sources/Models/WindowsSpecific/RegConfig.h")

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../../Sources/")

target_link_libraries(${PROJECT_NAME} PRIVATE 
    /usr/lib/x86_64-linux-gnu/libssl.a
    /usr/lib/x86_64-linux-gnu/libcrypto.a
)

# Post-build deployment based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/deploy_debug_vscode.sh ${CMAKE_CURRENT_SOURCE_DIR}/deploy
        COMMENT "Running debug deployment script after successful build."
    )
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/deploy_release.sh ${CMAKE_CURRENT_SOURCE_DIR}/deploy
        COMMENT "Running release deployment script after successful build."
    )
endif()

message(STATUS "Processing project ${PROJECT_NAME} has finished.")
