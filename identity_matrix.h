#pragma once
#include "matrix.h"

class IdentityMatrix : public Matrix {
    public:
        explicit IdentityMatrix(int dimension);
        const int* operator[](int row) const;

    protected:
        virtual void readFrom(std::istream& in) override;
};

void demonstrateIdentityMatrix();
