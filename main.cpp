#include <iostream>
#include <fstream>
#include "matrix.h"
#include <gtest/gtest.h>

int main(int argc, char** argv) {
    static const int data[4] = {1, 2, 3, 4};
    static const int dataB[4] = {4, 3, 2, 1};
    static const int data3[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};

    //Лаба 1
    Matrix a(2, data);
    Matrix b(2, dataB);
    Matrix s(3, data3);

    //Лаба 3
    std::cout << "\nEnter the matrix dimension and its elements row by row:\n";
    Matrix fromKeyboard;
    if (!(std::cin >> fromKeyboard)) {
        std::cerr << "Invalid or incomplete matrix.\n";
        return 1;
    }
    std::cout << "\nYou entered:\n" << fromKeyboard;

    {
        std::ofstream out("matrices.txt", std::ios::app);
        out << a << s << fromKeyboard;
        out.close();
        if (!out) {
            std::cerr << "Failed to write the text file.\n";
            return 1;
        }
    }
    {
        std::ifstream in("matrices.txt");
        if (!in) {
            std::cerr << "Failed to open the text file.\n";
            return 1;
        }
        std::cout << "\nMatrices from matrices.txt:\n";
        while (true) {
            in >> std::ws;
            if (in.peek() == std::char_traits<char>::eof()) {
                if (in.bad()) {
                    std::cerr << "Failed to read the text file.\n";
                    return 1;
                }
                break;
            }
            Matrix loaded;
            if (!(in >> loaded)) {
                std::cerr << "Corrupted record in the text file.\n";
                return 1;
            }
            std::cout << loaded << '\n';
        }
    }
    {
        std::ofstream out("matrices.bin", std::ios::binary | std::ios::app);
        a.saveBinary(out);
        s.saveBinary(out);
        fromKeyboard.saveBinary(out);
        out.close();
        if (!out) {
            std::cerr << "Failed to write the binary file.\n";
            return 1;
        }
    }
    {
        std::ifstream in("matrices.bin", std::ios::binary);
        if (!in) {
            std::cerr << "Failed to open the binary file.\n";
            return 1;
        }
        std::cout << "\nMatrices from matrices.bin:\n";
        while (in.peek() != std::char_traits<char>::eof()) {
            Matrix loaded;
            loaded.loadBinary(in);
            if (!in) {
                std::cerr << "Corrupted record in the binary file.\n";
                return 1;
            }
            std::cout << loaded << '\n';
        }
        if (in.bad()) {
            std::cerr << "Failed to read the binary file.\n";
            return 1;
        }
    }

    std::cout << "\n--- running tests ---\n\n";
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab3.*";
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}