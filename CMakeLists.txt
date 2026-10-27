cmake_minimum_required(VERSION 3.14)
project(MatrixLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(matrix_app matrix.cpp main.cpp test.cpp)
target_link_libraries(matrix_app PRIVATE gtest)  