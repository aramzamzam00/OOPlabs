#include <iostream>
#include <gtest/gtest.h>

void demonstrateIdentityMatrix();
void demonstrateRectangularMatrix();


int main(int argc, char** argv) {
    demonstrateIdentityMatrix();
    demonstrateRectangularMatrix();

    std::cout << "\n--- running tests ---\n\n";
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab3.*";
    return RUN_ALL_TESTS();
}