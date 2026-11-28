#include "identity_matrix.h"
#include <iostream>
#include <utility>

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

void IdentityMatrix::readFrom(std::istream& in) {
    Matrix value;
    if (!(in >> value)) {
        return;
    }
    for (int i = 0; i < value.getDimension(); ++i) {
        for (int j = 0; j < value.getDimension(); ++j) {
            if (value[i][j] != (i == j ? 1 : 0)) {
                in.setstate(std::ios::failbit);
                return;
            }
        }
    }
    Matrix::operator=(std::move(value));
}
