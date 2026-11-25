#include <iostream>
#include <gtest/gtest.h>

void demonstrateMatrices();

int main(int argc, char** argv) {
    demonstrateMatrices();

    std::cout << "\n--- running tests ---\n\n";
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab3.*";
    return RUN_ALL_TESTS();
}