#include <iostream>

#include "Matrix.h"

int main() {
    static const int data[4] = {1, 2, 3, 4};
    static const int dataB[4] = {4, 3, 2, 1};
    static const int data3[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};

    Matrix a(2, data);
    Matrix b(2, dataB);

    std::cout << "a:\n" << a;
    std::cout << "\nb:\n" << b;

    std::cout << "\na + b:\n" << a + b;
    std::cout << "\na - b:\n" << a - b;

    std::cout << "\na[0][1]: " << a[0][1] << "\n";

    Matrix c = a;
    c[1][0] = 9;
    std::cout << "\nc = a, then c[1][0] = 9:\n" << c;
    std::cout << "\na unchanged:\n" << a;

    std::cout << "\na() (determinant): " << a() << "\n";

    Matrix s(3, data3);
    std::cout << "\ns (3x3):\n" << s;
    std::cout << "\ns() (determinant): " << s() << "\n";

    Matrix d(2, dataB);
    d = a;
    d[0][0] = 100;
    std::cout << "\nd = a, then d[0][0] = 100:\n" << d;
    std::cout << "\na unchanged:\n" << a;

    return 0;
}