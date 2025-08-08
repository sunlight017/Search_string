cmake_minimum_required(VERSION 3.16)
project(SearchEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    "src/*.cpp"
    "include/*.h"
)

add_executable(search_engine ${SOURCES})

target_include_directories(search_engine PRIVATE include)
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

if(WIN32)
    target_compile_definitions(search_engine PRIVATE _CRT_SECURE_NO_WARNINGS)
    find_package(filesystem REQUIRED)
    target_link_libraries(search_engine PRIVATE std::filesystem)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()
configure_file(config.json ${CMAKE_BINARY_DIR}/bin/config.json COPYONLY)
configure_file(requests.json ${CMAKE_BINARY_DIR}/bin/requests.json COPYONLY)
