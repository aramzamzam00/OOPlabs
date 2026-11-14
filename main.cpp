#include <iostream>
#include "matrix.h"
#include <gtest/gtest.h>

int main(int argc, char** argv) {
     static const int data[4] = {
        1, 2,
        3, 4
    };

    static const int data3[9] = {
        2, 0, 1,
        1, 3, 2,
        3, 1, 4
    };

    Matrix a(2, data);
    Matrix s(3, data3);
    Matrix fromKeyboard;

    Matrix textMatrix;
    Matrix binaryMatrix;

    std::cout << "Text record 1 loaded: "
              << textMatrix.loadTextMatrix(1) << '\n';

    std::cout << "Binary record 1 loaded: "
              << binaryMatrix.loadBinaryMatrix(1) << '\n';

    Matrix::printMatrices(&textMatrix, 1);
    Matrix::printMatrices(&binaryMatrix, 1);

    Matrix transposed = textMatrix.transpose();

    std::cout << "Loaded text matrix transposed:\n";
    Matrix::printMatrices(&transposed, 1);

    std::cout
        << "Enter the matrix dimension "
        << "and its elements row by row:\n";

    std::cout
        << "Keyboard input accepted: "
        << fromKeyboard.readMatrix() << '\n';

    Matrix::printMatrices(&fromKeyboard, 1);

    std::cout << "Text a saved: "
              << a.saveTextMatrix() << '\n';

    std::cout << "Text s saved: "
              << s.saveTextMatrix() << '\n';

    std::cout << "Text keyboard matrix saved: "
              << fromKeyboard.saveTextMatrix() << '\n';

    std::cout << "Binary a saved: "
              << a.saveBinaryMatrix() << '\n';

    std::cout << "Binary s saved: "
              << s.saveBinaryMatrix() << '\n';

    std::cout << "Binary keyboard matrix saved: "
              << fromKeyboard.saveBinaryMatrix() << '\n';

    int textCount = 0;
    int binaryCount = 0;

    Matrix* textMatrices = Matrix::loadTextMatrices(textCount);
    Matrix* binaryMatrices = Matrix::loadBinaryMatrices(binaryCount);

    std::cout << "All matrices from matrices.txt:\n";
    Matrix::printMatrices(textMatrices, textCount);

    std::cout << "All matrices from matrices.bin:\n";
    Matrix::printMatrices(binaryMatrices, binaryCount);

    Matrix::deleteMatrices(textMatrices, textCount);
    Matrix::deleteMatrices(binaryMatrices, binaryCount);

    std::cout << "\n--- running tests ---\n\n";
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab3.*";
    return RUN_ALL_TESTS();
}