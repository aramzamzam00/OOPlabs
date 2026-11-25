#include <iostream>
#include "matrix.h"
#include <gtest/gtest.h>

bool saveText(const Matrix* items, int count, const char* path = nullptr);
bool saveBinary(const Matrix* items, int count, const char* path = nullptr);
void printSavedMatrices();

int main(int argc, char** argv) {
    std::cout << "Previously saved matrices:\n";
    printSavedMatrices();

    Matrix typed;
    std::cout << "Enter the dimension and the elements row by row:\n";
    if (std::cin >> typed) {
        std::cout << "Entered matrix:\n" << typed;
        std::cout << "Text save: " << saveText(&typed, 1) << '\n';
        std::cout << "Binary save: " << saveBinary(&typed, 1) << '\n';

        std::cout << "Matrices after saving:\n";
        printSavedMatrices();
    } else {
        std::cerr << "No complete matrix entered.\n";
    }

    std::cout << "\n--- running tests ---\n\n";
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab3.*";
    return RUN_ALL_TESTS();
}