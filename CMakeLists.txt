cmake_minimum_required(VERSION 3.16)
project(SearchEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)


file(GLOB_RECURSE SOURCES 
    CONFIGURE_DEPENDS 
    "src/*.cpp"
    "include/*.h"
)

add_executable(search_engine ${SOURCES})


target_include_directories(search_engine 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(search_engine 
    PRIVATE 
    nlohmann_json::nlohmann_json
)

set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
if(EXISTS ${RESOURCES_DIR} AND IS_DIRECTORY ${RESOURCES_DIR})
    message(STATUS "Copying resources from: ${RESOURCES_DIR}")
    file(COPY ${RESOURCES_DIR} DESTINATION ${CMAKE_BINARY_DIR}/bin)
    file(COPY 
        ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        ${CMAKE_CURRENT_SOURCE_DIR}/requests.json
        DESTINATION ${CMAKE_BINARY_DIR}/bin
    )
else()
    message(WARNING "Resources directory not found at: ${RESOURCES_DIR}")
endif()

if(WIN32)
    target_compile_definitions(search_engine 
        PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
endif()

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

message(STATUS "Project configuration complete")
message(STATUS "Binary output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
