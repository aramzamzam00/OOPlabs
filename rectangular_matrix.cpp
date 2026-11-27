#include "rectangular_matrix.h"
#include <iostream>
#include <stdexcept>

static int storageDimension(int rows, int columns) {
    if (rows <= 0 || columns <= 0) {
        throw std::invalid_argument("Invalid rectangular matrix dimensions");
    }
    return rows > columns ? rows : columns;
}

RectangularMatrix::RectangularMatrix(int rows, int columns)
    : Matrix(storageDimension(rows, columns)), rows(rows), columns(columns) {
}

RectangularMatrix::RectangularMatrix(int rows, int columns, const int* elements)
    : RectangularMatrix(rows, columns) {
    if (!elements) {
        throw std::invalid_argument("Missing matrix elements");
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            (*this)[i][j] = elements[i * columns + j];
        }
    }
}

int RectangularMatrix::getRows() const {
    return rows;
}

int RectangularMatrix::getColumns() const {
    return columns;
}

void RectangularMatrix::checkIndex(int row, int col) const {
    if (row < 0 || row >= rows || col < 0 || col >= columns) {
        throw std::out_of_range("Rectangular matrix index out of range");
    }
}

int RectangularMatrix::getElement(int row, int col) const {
    checkIndex(row, col);
    return Matrix::getElement(row, col);
}

RectangularMatrix RectangularMatrix::setElement(int row, int col, int value) const {
    checkIndex(row, col);
    RectangularMatrix result(*this);
    result[row][col] = value;
    return result;
}

RectangularMatrix RectangularMatrix::transpose() const {
    RectangularMatrix result(columns, rows);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            result[j][i] = (*this)[i][j];
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& out, const RectangularMatrix& matrix) {
    out << matrix.getRows() << ' ' << matrix.getColumns() << '\n';
    for (int i = 0; i < matrix.getRows(); ++i) {
        for (int j = 0; j < matrix.getColumns(); ++j) {
            if (j > 0) {
                out << '\t';
            }
            out << matrix.getElement(i, j);
        }
        out << '\n';
    }
    return out;
}

void demonstrateRectangularMatrix() {
    const int data[] = {1, 2, 3, 4, 5, 6};
    RectangularMatrix original(2, 3, data);
    RectangularMatrix copy(original);
    copy = copy.setElement(0, 0, 9);

    std::cout << "Rectangular matrix:\n" << original;
    std::cout << "Changed copy:\n" << copy;
    std::cout << "Transposed original:\n" << original.transpose();
}