#include "identity_matrix.h"
#include <iostream>

IdentityMatrix::IdentityMatrix(int dimension) : Matrix(dimension) {
    for (int i = 0; i < dimension; ++i) {
        Matrix::operator[](i)[i] = 1;
    }
}

const int* IdentityMatrix::operator[](int row) const {
    return Matrix::operator[](row);
}

void demonstrateIdentityMatrix() {
    IdentityMatrix original(3);
    IdentityMatrix copy(original);

    std::cout << "Identity matrix:\n" << original;
    std::cout << "Copy:\n" << copy;
    std::cout << "Determinant: " << copy() << '\n';
}
