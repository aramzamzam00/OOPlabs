#pragma once
#include "matrix.h"

class RectangularMatrix : public Matrix {
    public:
        RectangularMatrix(int rows, int columns);
        RectangularMatrix(int rows, int columns, const int* elements);
        RectangularMatrix(const RectangularMatrix& other) = default;
        RectangularMatrix& operator=(const RectangularMatrix& other) = default;

        int getRows() const;
        int getColumns() const;
        int getElement(int row, int col) const;
        RectangularMatrix setElement(int row, int col, int value) const;
        RectangularMatrix transpose() const;

    private:
        int rows;
        int columns;
        void checkIndex(int row, int col) const;
        using Matrix::operator[];
};

std::ostream& operator<<(std::ostream& out, const RectangularMatrix& matrix);