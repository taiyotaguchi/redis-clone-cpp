cmake_minimum_required(VERSION 3.20)
project(redis-clone-cpp VERSION 0.1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

include(FetchContent)

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
add_executable(myproject
  playground/gtesttest.cpp
)

target_link_libraries(myproject PRIVATE spdlog::spdlog)
target_link_libraries(myproject PRIVATE GTest::gtest_main)
