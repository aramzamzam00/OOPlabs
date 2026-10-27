#include <iostream>

#include <gtest/gtest.h>

#include "Matrix.h"

int main(int argc, char** argv) {
    static const int data[4] = {1, 2, 3, 4};
    static const int dataB[4] = {4, 3, 2, 1};

    Matrix empty;
    Matrix zero(2);
    std::cout << "empty dim: " << empty.getDimension() << "\n";
    std::cout << "empty matrix \n" << zero << "\n";

    Matrix filled(2, data);
    Matrix k(2, dataB);

    char* str = filled.toString();


    std::cout << "filled:\n" << filled;
    std::cout << "\ntoString:\n" << str << "\n";
    std::cout << "\nget(1,0): " << filled.getElement(1, 0)
              << ", flat[0]: " << filled.getElements()[0]
              << ", dim: " << filled.getDimension() << "\n";

    std::cout << "\nsetElement(0,0,9):\n" << filled.setElement(0, 0, 9);
    std::cout << "\nsetElements:\n"     << filled.setElements(dataB);
    std::cout << "\nsetDimension(3):\n" << filled.setDimension(3);

    std::cout << "\ntransposition:\n"  << filled.transposition();
    std::cout << "\nsummary:\n"        << filled.summary(k);
    std::cout << "\nmatrixMultiply:\n" << filled.matrixMultiply(k);
    std::cout << "\nnumberMultiply:\n" << filled.numberMultiply(3);

    std::cout << "\n--- running tests ---\n\n";
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}