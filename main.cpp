#include <iostream>
#include <gtest/gtest.h>

void demonstrateRectangularMatrix();

int main(int argc, char** argv) {
    demonstrateRectangularMatrix();

    std::cout << "\n--- running tests ---\n\n";
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab5.*";
    return RUN_ALL_TESTS();
}