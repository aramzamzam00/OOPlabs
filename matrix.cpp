#include "matrix.h"
#include <iostream>
#include <stdexcept>
#include <cstring>

namespace {
    int indexOf(int row, int col, int dim) {
        return row * dim + col;
    }
}
//конструкторы
Matrix::Matrix() : dimension(0), elements(nullptr) {}
Matrix::Matrix(int newDimension) : dimension(newDimension), elements(new int[newDimension*newDimension]) {
    for (int i = 0; i < dimension*dimension; i++) elements[i] = 0;
}
Matrix::Matrix(int newDimension, const int* newElements) 
    : dimension(newDimension), elements(new int[newDimension*newDimension]) 
{
    for (int i = 0; i < dimension*dimension; i++) elements[i] = newElements[i];
}
// 5
Matrix::~Matrix() {
    delete[] elements;
}

Matrix::Matrix(const Matrix& other) 
    : dimension(other.dimension), elements(new int[other.dimension * other.dimension])
{
    for (int i = 0; i < dimension * dimension; ++i) {
        elements[i] = other.elements[i];
    }
}

Matrix::Matrix(Matrix&& other)
    : dimension(other.dimension), elements(other.elements)
{
    other.dimension = 0;
    other.elements = nullptr;
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) {
        return *this;
    }
    int* newElements = new int[other.dimension * other.dimension];
    for (int i = 0; i < other.dimension * other.dimension; ++i) {
        newElements[i] = other.elements[i];
    }
    delete[] elements;
    elements = newElements;
    dimension = other.dimension;
    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) {
    if (this == &other) {
        return *this;
    }
    delete[] elements;
    dimension = other.dimension;
    elements = other.elements;
    other.dimension = 0;
    other.elements = nullptr;
    return *this;
}
//геттеры и сеттеры
int Matrix::getDimension() const {
    return dimension;
}

Matrix Matrix::setDimension(int newDimension) const {
    Matrix result(newDimension);
    int minimalDim = (newDimension < dimension) ? newDimension : dimension;
    for (int i = 0; i < minimalDim; ++i) {
        for (int j = 0; j < minimalDim; ++j) {
            result.elements[indexOf(i, j, newDimension)] =
                elements[indexOf(i, j, dimension)];
        }
    }
    return result;
}

const int* Matrix::getElements() const {
    return elements;
}

Matrix Matrix::setElements(const int* newElements) const {
    return Matrix(dimension, newElements);
}

int Matrix::getElement(int row, int col) const {
    return elements[indexOf(row, col, dimension)];
}

Matrix Matrix::setElement(int row, int col, int value) const {
    Matrix result(*this);
    result.elements[indexOf(row, col, dimension)] = value;
    return result;
}
// операции над матрицами
Matrix Matrix::transposition() const {
    Matrix result(dimension);
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            result.elements[indexOf(i, j, dimension)] =
                elements[indexOf(j, i, dimension)];
        }
    }
    return result;
}

Matrix Matrix::summary(const Matrix& other) const {
    if (dimension != other.dimension) {
        throw std::invalid_argument("summary: different matrix dimensions");
    }
    Matrix result(dimension);
    for (int i = 0; i < dimension * dimension; ++i) {
        result.elements[i] = elements[i] + other.elements[i];
    }
    return result;
}

Matrix Matrix::matrixMultiply(const Matrix& other) const {
    if (dimension != other.dimension) {
        throw std::invalid_argument("matrixMultiply: different matrix dimensions");
    }
    Matrix result(dimension);
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            int sum = 0;
            for (int k = 0; k < dimension; ++k) {
                sum += elements[indexOf(i, k, dimension)] *
                       other.elements[indexOf(k, j, dimension)];
            }
            result.elements[indexOf(i, j, dimension)] = sum;
        }
    }
    return result;
}

Matrix Matrix::numberMultiply(int number) const {
    Matrix result(dimension);
    for (int i = 0; i < dimension * dimension; ++i) {
        result.elements[i] = elements[i] * number;
    }
    return result;
}

Matrix Matrix::operator+(const Matrix& other) const {
    return summary(other);
}

Matrix operator-(const Matrix& leftMatrix, const Matrix& rightMatrix) {
    if (leftMatrix.dimension != rightMatrix.dimension) {
        throw std::invalid_argument("operator-: different matrix dimensions");
    }
    Matrix result(leftMatrix.dimension);
    for (int i = 0; i < leftMatrix.dimension * leftMatrix.dimension; ++i) {
        result.elements[i] = leftMatrix.elements[i] - rightMatrix.elements[i];
    }
    return result;
}

int* Matrix::operator[](int row) {
    if (row < 0 || row >= dimension) {
        throw std::out_of_range("operator[]: row index out of range");
    }
    return elements + row * dimension;
}

const int* Matrix::operator[](int row) const {
    if (row < 0 || row >= dimension) {
        throw std::out_of_range("operator[]: row index out of range");
    }
    return elements + row * dimension;
}

int Matrix::operator()() const {
    if (dimension == 0) {
        throw std::invalid_argument("determinant: empty matrix");
    }
    const int n = dimension;
    long long* a = new long long[n * n];
    for (int i = 0; i < n * n; ++i) {
        a[i] = elements[i];
    }
    long long prev = 1;
    int sign = 1;
    for (int k = 0; k < n - 1; ++k) {
        if (a[indexOf(k, k, n)] == 0) {
            int swapRow = -1;
            for (int i = k + 1; i < n; ++i) {
                if (a[indexOf(i, k, n)] != 0) {
                    swapRow = i;
                    break;
                }
            }
            if (swapRow == -1) {
                delete[] a;
                return 0;
            }
            for (int j = 0; j < n; ++j) {
                long long tmp = a[indexOf(k, j, n)];
                a[indexOf(k, j, n)] = a[indexOf(swapRow, j, n)];
                a[indexOf(swapRow, j, n)] = tmp;
            }
            sign = -sign;
        }
        for (int i = k + 1; i < n; ++i) {
            for (int j = k + 1; j < n; ++j) {
                a[indexOf(i, j, n)] =
                    (a[indexOf(i, j, n)] * a[indexOf(k, k, n)] -
                     a[indexOf(i, k, n)] * a[indexOf(k, j, n)]) / prev;
            }
        }
        prev = a[indexOf(k, k, n)];
    }
    long long det = sign * a[indexOf(n - 1, n - 1, n)];
    delete[] a;
    return static_cast<int>(det);
}

bool operator==(const Matrix& leftMatrix, const Matrix& rightMatrix) {
    if (leftMatrix.getDimension() != rightMatrix.getDimension()) return false;
    const int n = leftMatrix.getDimension();
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (leftMatrix.getElement(i, j) != rightMatrix.getElement(i, j))
                return false;
    return true;
}

bool operator!=(const Matrix& leftMatrix, const Matrix& rightMatrix) {
    return !(leftMatrix == rightMatrix);
}

std::ostream& operator<<(std::ostream& ostream, const Matrix& matrix) {
    const int n = matrix.getDimension();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            ostream << matrix.getElement(i, j);
            if (j + 1 < n) ostream << '\t';
        }
        if (i + 1 < n) ostream << '\n';
    }
    return ostream;
}

char* Matrix::toString() const {
    if (dimension == 0) {
        char* empty = new char[1];
        empty[0] = '\0';
        return empty;
    }
    int maxLen = dimension * dimension * 12;
    char* buffer = new char[maxLen];
    int pos = 0;

    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            char numStr[12];
            sprintf(numStr, "%d", elements[i * dimension + j]);

            int len = strlen(numStr);
            for (int k = 0; k < len; ++k) {
                buffer[pos++] = numStr[k];
            }

            if (j+1 < dimension) {
                buffer[pos++] = '\t';
            }
        }
        
        if (i + 1 < dimension) {
            buffer[pos++] = '\n';
        }
    }
    buffer[pos] = '\0';
    return buffer;
}