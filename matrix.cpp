#include "matrix.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <limits>
#include <new>
#include <utility>
#include <algorithm>

static int indexOf(int row, int col, int dim) {
    return row * dim + col;
}

static const char textFileName[] = MATRIX_PROJECT_DIR "/matrices.txt";
static const char binaryFileName[] = MATRIX_PROJECT_DIR "/matrices.bin";

// Конструкторы

Matrix::Matrix()
    : dimension(0), elements(nullptr) {
}

Matrix::Matrix(int newDimension)
    : dimension(newDimension), elements(nullptr) {
    if (newDimension < 0 ||
        (newDimension > 0 &&
         newDimension > std::numeric_limits<int>::max() / newDimension)) {
        throw std::invalid_argument("Invalid matrix dimension");
    }

    if (dimension > 0) {
        elements = new int[dimension * dimension];
    }

    for (int i = 0; i < dimension * dimension; ++i) {
        elements[i] = 0;
    }
}

Matrix::Matrix(int newDimension, const int* newElements)
    : Matrix(newDimension) {
    if (dimension > 0 && !newElements) {
        throw std::invalid_argument("Missing matrix elements");
    }

    for (int i = 0; i < dimension * dimension; ++i) {
        elements[i] = newElements[i];
    }
}

// Правило пяти

Matrix::~Matrix() {
    delete[] elements;
}

Matrix::Matrix(const Matrix& other)
    : dimension(other.dimension),
      elements(new int[other.dimension * other.dimension]) {
    for (int i = 0; i < dimension * dimension; ++i) {
        elements[i] = other.elements[i];
    }
}

Matrix::Matrix(Matrix&& other)
    : dimension(other.dimension), elements(other.elements) {
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

// Геттеры и сеттеры

int Matrix::getDimension() const {
    return dimension;
}

Matrix Matrix::setDimension(int newDimension) const {
    Matrix result(newDimension);

    int minimalDim =
        newDimension < dimension ? newDimension : dimension;

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

// Операции над матрицами

Matrix Matrix::transpose() const {
    Matrix result(dimension);

    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            result.elements[indexOf(i, j, dimension)] =
                elements[indexOf(j, i, dimension)];
        }
    }

    return result;
}

Matrix Matrix::add(const Matrix& other) const {
    if (dimension != other.dimension) {
        throw std::invalid_argument(
            "summary: different matrix dimensions"
        );
    }

    Matrix result(dimension);

    for (int i = 0; i < dimension * dimension; ++i) {
        result.elements[i] = elements[i] + other.elements[i];
    }

    return result;
}

Matrix Matrix::multiply(const Matrix& other) const {
    if (dimension != other.dimension) {
        throw std::invalid_argument(
            "matrixMultiply: different matrix dimensions"
        );
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

Matrix Matrix::multiplyByNumber(int number) const {
    Matrix result(dimension);

    for (int i = 0; i < dimension * dimension; ++i) {
        result.elements[i] = elements[i] * number;
    }

    return result;
}

// Лабораторная № 2

Matrix Matrix::operator+(const Matrix& other) const {
    return add(other);
}

Matrix operator-(
    const Matrix& leftMatrix,
    const Matrix& rightMatrix
) {
    if (leftMatrix.dimension != rightMatrix.dimension) {
        throw std::invalid_argument(
            "operator-: different matrix dimensions"
        );
    }

    Matrix result(leftMatrix.dimension);

    for (int i = 0;
         i < leftMatrix.dimension * leftMatrix.dimension;
         ++i) {
        result.elements[i] =
            leftMatrix.elements[i] - rightMatrix.elements[i];
    }

    return result;
}

int* Matrix::operator[](int row) {
    if (row < 0 || row >= dimension) {
        throw std::out_of_range(
            "operator[]: row index out of range"
        );
    }

    return elements + row * dimension;
}

const int* Matrix::operator[](int row) const {
    if (row < 0 || row >= dimension) {
        throw std::out_of_range(
            "operator[]: row index out of range"
        );
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

                a[indexOf(k, j, n)] =
                    a[indexOf(swapRow, j, n)];

                a[indexOf(swapRow, j, n)] = tmp;
            }

            sign = -sign;
        }

        for (int i = k + 1; i < n; ++i) {
            for (int j = k + 1; j < n; ++j) {
                a[indexOf(i, j, n)] =
                    (
                        a[indexOf(i, j, n)] *
                        a[indexOf(k, k, n)] -
                        a[indexOf(i, k, n)] *
                        a[indexOf(k, j, n)]
                    ) / prev;
            }
        }

        prev = a[indexOf(k, k, n)];
    }

    long long det = sign * a[indexOf(n - 1, n - 1, n)];

    delete[] a;

    return static_cast<int>(det);
}

// Лабораторная 3

void Matrix::saveBinary(std::ostream& out) const {
    if (dimension <= 0) {
        out.setstate(std::ios::failbit);
        return;
    }

    out.write(
        reinterpret_cast<const char*>(&dimension),
        sizeof(dimension)
    );

    out.write(
        reinterpret_cast<const char*>(elements),
        static_cast<std::streamsize>(sizeof(int)) *
            dimension * dimension
    );
}

static void readRecord(std::istream& in, Matrix& matrix, bool binary) {
    int n = 0;
    if (binary) {
        in.read(reinterpret_cast<char*>(&n), sizeof(n));
    } else {
        in >> n;
    }

    if (!in || n <= 0 || n > std::numeric_limits<int>::max() / n) {
        in.setstate(std::ios::failbit);
        return;
    }

    Matrix temporary;
    int used = 0;
    int side = n < 8 ? n : 8;

    try {
        while (used < n * n) {
            Matrix next(side);
            if (used > 0) {
                std::copy_n(temporary.getElements(), used, next[0]);
            }
            temporary = std::move(next);

            const int end = side * side;
            int* data = temporary[0];
            if (binary) {
                in.read(reinterpret_cast<char*>(data + used),
                        static_cast<std::streamsize>(sizeof(int)) * (end - used));
            } else {
                for (int i = used; i < end && in; ++i) {
                    in >> data[i];
                }
            }
            if (!in) {
                return;
            }
            used = end;
            side = std::min(n, side * 2);
        }
    } catch (const std::bad_alloc&) {
        in.setstate(std::ios::failbit);
        return;
    }

    matrix = std::move(temporary);
}

void Matrix::loadBinary(std::istream& in) {
    readRecord(in, *this, true);
}

static bool saveRecords(const char* path, const Matrix* items, int count, bool binary) {
    if (count < 0 || (count > 0 && !items)) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if (items[i].getDimension() == 0) {
            return false;
        }
    }

    std::ofstream out(path, binary ? std::ios::binary | std::ios::app : std::ios::app);
    for (int i = 0; i < count && out; ++i) {
        if (binary) {
            items[i].saveBinary(out);
        } else {
            out << '\n' << items[i];
        }
    }
    out.close();
    return !out.fail();
}

bool saveText(const Matrix* items, int count, const char* path) {
    return saveRecords(path ? path : textFileName, items, count, false);
}

bool saveBinary(const Matrix* items, int count, const char* path) {
    return saveRecords(path ? path : binaryFileName, items, count, true);
}

// Загрузка всех матриц из файла

static Matrix* loadRecords(const char* path, int& count, bool binary) {
    count = 0;
    std::ifstream in(path, binary ? std::ios::binary : std::ios::in);
    Matrix* items = nullptr;
    int capacity = 0;

    try {
        while (in) {
            if (!binary) {
                in >> std::ws;
            }
            if (in.peek() == std::char_traits<char>::eof()) {
                if (in.eof() && !in.bad()) {
                    return items;
                }
                break;
            }

            Matrix current;
            if (binary) {
                current.loadBinary(in);
            } else {
                in >> current;
            }
            if (!in) {
                break;
            }

            if (count == capacity) {
                if (capacity > std::numeric_limits<int>::max() / 2) {
                    break;
                }
                capacity = capacity == 0 ? 4 : capacity * 2;
                Matrix* larger = new Matrix[capacity];
                for (int i = 0; i < count; ++i) {
                    larger[i] = std::move(items[i]);
                }
                delete[] items;
                items = larger;
            }
            items[count++] = std::move(current);
        }
    } catch (const std::bad_alloc&) {
        in.setstate(std::ios::failbit);
    }

    delete[] items;
    count = -1;
    std::cerr << "Cannot read matrix file: " << path << '\n';
    return nullptr;
}

Matrix* loadText(int& count, const char* path) {
    return loadRecords(path ? path : textFileName, count, false);
}

Matrix* loadBinary(int& count, const char* path) {
    return loadRecords(path ? path : binaryFileName, count, true);
}

static void printMatrices(const Matrix* matrices, int count) {
    if (count < 0) {
        std::cout << "Matrices could not be loaded.\n";
        return;
    }

    if (!matrices || count == 0) {
        std::cout << "No matrices loaded.\n";
        return;
    }

    for (int i = 0; i < count; ++i) {
        std::cout << "Matrix " << i + 1 << ":\n";

        if (matrices[i].getDimension() == 0) {
            std::cout << "Empty matrix.\n";
        } else {
            std::cout << matrices[i] << '\n';
        }
    }
}

void printSavedMatrices() {
    int count = 0;
    std::cout << "Matrices from matrices.txt:\n";
    Matrix* items = loadText(count, nullptr);
    printMatrices(items, count);
    delete[] items;

    std::cout << "Matrices from matrices.bin:\n";
    items = loadBinary(count, nullptr);
    printMatrices(items, count);
    delete[] items;
}

bool operator==(
    const Matrix& leftMatrix,
    const Matrix& rightMatrix
) {
    if (leftMatrix.getDimension() != rightMatrix.getDimension()) {
        return false;
    }

    const int n = leftMatrix.getDimension();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (leftMatrix.getElement(i, j) !=
                rightMatrix.getElement(i, j)) {
                return false;
            }
        }
    }

    return true;
}

bool operator!=(
    const Matrix& leftMatrix,
    const Matrix& rightMatrix
) {
    return !(leftMatrix == rightMatrix);
}


std::ostream& operator<<(std::ostream& out, const Matrix& matrix) {
    const int n = matrix.getDimension();

    if (n <= 0) {
        out.setstate(std::ios::failbit);
        return out;
    }

    out << n << '\n';

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            out << matrix.getElement(i, j);

            if (j + 1 < n) {
                out << '\t';
            }
        }

        out << '\n';
    }

    return out;
}


std::istream& operator>>(std::istream& in, Matrix& matrix) {
    readRecord(in, matrix, false);
    return in;
}


char* Matrix::formatElements() const {
    if (dimension == 0) {
        char* empty = new char[1];
        empty[0] = '\0';

        return empty;
    }

    std::size_t maxLen =
        static_cast<std::size_t>(dimension) *
        dimension * 12 + 1;

    char* buffer = new char[maxLen];
    std::size_t pos = 0;

    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            char numStr[12];

            std::snprintf(
                numStr,
                sizeof(numStr),
                "%d",
                elements[i * dimension + j]
            );

            std::size_t len = std::strlen(numStr);

            for (std::size_t k = 0; k < len; ++k) {
                buffer[pos++] = numStr[k];
            }

            if (j + 1 < dimension) {
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