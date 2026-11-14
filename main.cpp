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

    std::cout << "\n--- running tests ---\n\n";
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab3.*";
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}