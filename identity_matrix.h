#pragma once
#include "matrix.h"

class IdentityMatrix : public Matrix {
    public:
        explicit IdentityMatrix(int dimension);
        const int* operator[](int row) const;
};

void demonstrateIdentityMatrix();