#include <iostream>
#include <fstream>

#include <gtest/gtest.h>

#include "Matrix.h"

int main(int argc, char** argv) {
    static const int data[4] = {1, 2, 3, 4};
    static const int dataB[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};

    Matrix a(2, data);
    Matrix b(3, dataB);

    std::cout << "enter dimension and elements from keyboard:\n";
    Matrix fromKeyboard;
    std::cin >> fromKeyboard;
    std::cout << "\nyou entered:\n" << fromKeyboard;

    {
        std::ofstream out("matrices.txt");
        out << 2 << '\n';
        out << a << b;
    }
    {
        std::ifstream in("matrices.txt");
        int count = 0;
        in >> count;
        std::cout << "\nloaded " << count << " matrices from matrices.txt:\n";
        for (int i = 0; i < count; ++i) {
            Matrix m;
            in >> m;
            std::cout << m << '\n';
        }
    }

    {
        std::ofstream out("matrices.bin", std::ios::binary);
        int count = 2;
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        a.saveBinary(out);
        b.saveBinary(out);
    }
    {
        std::ifstream in("matrices.bin", std::ios::binary);
        int count = 0;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        std::cout << "loaded " << count << " matrices from matrices.bin:\n";
        for (int i = 0; i < count; ++i) {
            Matrix m;
            m.loadBinary(in);
            std::cout << m << '\n';
        }
    }

    std::cout << "\n--- running tests ---\n\n";
    ::testing::GTEST_FLAG(filter) = "MatrixTestsLab3.*";
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}