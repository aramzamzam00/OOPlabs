cmake_minimum_required(VERSION 3.14)
project(MatrixLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# Класс Matrix
add_library(matrix matrix.cpp)

target_compile_definitions(
    matrix PRIVATE MATRIX_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(
    matrix PRIVATE MATRIX_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(
    matrix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# Демонстрационная программа
add_executable(matrix_app main.cpp)
target_link_libraries(matrix_app PRIVATE matrix)

# Тесты
include(CTest)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

target_sources(matrix_app PRIVATE test.cpp)
target_link_libraries(matrix_app PRIVATE gtest)

if(BUILD_TESTING)
    add_executable(matrix_tests test.cpp)
    target_link_libraries(matrix_tests PRIVATE matrix gtest_main)

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test-data")

    add_test(NAME matrix_tests COMMAND matrix_tests)

    set_tests_properties(
        matrix_tests
        PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test-data"
    )
endif()