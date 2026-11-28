#include <iostream>
#include <gtest/gtest.h>

#include "rectangular_matrix.h"
#include "matrix_queue.h"

int main(int argc, char** argv) {
    demonstrateRectangularMatrix();
    demonstrateMatrixQueue();

    std::cout << "\n--- running tests ---\n\n";
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab5.*";
    return RUN_ALL_TESTS();
}