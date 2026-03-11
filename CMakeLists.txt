cmake_minimum_required(VERSION 3.20)
project(redis-clone-cpp VERSION 0.1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
add_compile_options(-Wall -Wextra -Wpedantic)

if (CMAKE_BUILD_TYPE STREQUAL "Debug") 
  add_compile_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)  
  add_link_options(-fsanitize=address -fsanitize=undefined)
endif()

include(FetchContent)

# Modules
FetchContent_Declare(
  spdlog
  URL https://github.com/gabime/spdlog/archive/refs/tags/v1.13.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_Declare(
  googletest 
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_MakeAvailable(spdlog googletest)

# Main binary
add_executable(server
  playground/servertest.cpp
)

add_executable(client
  playground/clienttest.cpp
)

target_link_libraries(server PRIVATE spdlog::spdlog)
target_link_libraries(client PRIVATE spdlog::spdlog)

# Testing binary
enable_testing()

add_executable(test_myproject
  tests/cpp/test_general.cpp
)

target_link_libraries(test_myproject PRIVATE GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(test_myproject)
