cmake_minimum_required(VERSION 3.30.1)
project(search_engine)

set(CMAKE_CXX_STANDARD 17)
target_include_directories(search_engine PRIVATE include)
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()
add_executable(search_engine
    src/main.cpp
    src/ConverterJSON.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp
)

target_include_directories(search_engine PRIVATE include)
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

add_executable(search_engine_tests
    test/test_main.cpp
    test/test_invertedindex.cpp
    test/test_searchserver.cpp
    src/ConverterJSON.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp
)

target_include_directories(search_engine_tests PRIVATE include)
target_link_libraries(search_engine_tests PRIVATE
    nlohmann_json::nlohmann_json
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(search_engine_tests)
