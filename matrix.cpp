#include "matrix.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <limits>
#include <new>
#include <utility>

namespace {
    int indexOf(int row, int col, int dim) {
        return row * dim + col;
    }
}

static const char textFileName[] = "matrices.txt";
static const char binaryFileName[] = "matrices.bin";

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

// Лабораторная № 3: двоичные потоки

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

void Matrix::loadBinary(std::istream& in) {
    int n = 0;

    if (!in.read(reinterpret_cast<char*>(&n), sizeof(n))) {
        return;
    }

    if (n <= 0 || n > std::numeric_limits<int>::max() / n) {
        in.setstate(std::ios::failbit);
        return;
    }

    try {
        Matrix temporary(n);

        in.read(
            reinterpret_cast<char*>(temporary.elements),
            static_cast<std::streamsize>(sizeof(int)) * n * n
        );

        if (in) {
            *this = std::move(temporary);
        }
    } catch (const std::invalid_argument&) {
        in.setstate(std::ios::failbit);
    } catch (const std::bad_alloc&) {
        in.setstate(std::ios::failbit);
    }
}

// Ввод с клавиатуры

bool Matrix::readMatrix() {
    if (!(std::cin >> *this)) {
        std::cerr << "Invalid or incomplete matrix.\n";
        return false;
    }

    return true;
}

// Общий помощник сохранения

static bool saveMatrix(
    const Matrix& matrix,
    const char* fileName,
    bool binary
) {
    if (matrix.getDimension() <= 0) {
        std::cerr << "Cannot save an empty matrix.\n";
        return false;
    }

    try {
        std::ofstream out(
            fileName,
            binary
                ? std::ios::binary | std::ios::app
                : std::ios::app
        );

        if (!out) {
            std::cerr
                << "Failed to open file for writing: "
                << fileName << '\n';

            return false;
        }

        if (binary) {
            matrix.saveBinary(out);
        } else {
            // Разделяем записи, даже если существующий файл
            // не заканчивается пробелом или переводом строки.
            out << '\n' << matrix;
        }

        out.close();

        if (!out) {
            std::cerr
                << "Failed to write file: "
                << fileName << '\n';

            return false;
        }

        return true;
    } catch (const std::exception& error) {
        std::cerr
            << "Failed to save matrix: "
            << error.what() << '\n';

        return false;
    }
}

bool Matrix::saveTextMatrix() const {
    return saveMatrix(*this, textFileName, false);
}

bool Matrix::saveBinaryMatrix() const {
    return saveMatrix(*this, binaryFileName, true);
}

// Добавление матрицы в динамический массив

static void appendMatrix(
    Matrix*& matrices,
    int& count,
    int& capacity,
    const Matrix& matrix
) {
    if (count == capacity) {
        if (capacity > std::numeric_limits<int>::max() / 2) {
            throw std::length_error("Too many matrices");
        }

        int newCapacity = capacity == 0 ? 4 : capacity * 2;
        Matrix* newMatrices = new Matrix[newCapacity];

        try {
            for (int i = 0; i < count; ++i) {
                newMatrices[i] = matrices[i];
            }
        } catch (...) {
            delete[] newMatrices;
            throw;
        }

        delete[] matrices;

        matrices = newMatrices;
        capacity = newCapacity;
    }

    matrices[count] = matrix;
    ++count;
}

// Чтение одной записи из уже открытого файла.
// Полнота записи проверяется до выделения памяти.

static void readFileMatrix(
    std::ifstream& in,
    bool binary,
    Matrix& loaded
) {
    std::streampos position = in.tellg();

    int n = 0;

    if (binary) {
        in.read(reinterpret_cast<char*>(&n), sizeof(n));
    } else {
        in >> n;
    }

    if (!in ||
        n <= 0 ||
        n > std::numeric_limits<int>::max() / n) {
        throw std::runtime_error("Invalid matrix dimension");
    }

    if (binary) {
        std::streampos dataPosition = in.tellg();

        in.seekg(0, std::ios::end);

        std::streamoff bytes =
            static_cast<std::streamoff>(sizeof(int)) * n * n;

        if (!in || in.tellg() - dataPosition < bytes) {
            throw std::runtime_error("Incomplete binary record");
        }
    } else {
        int value = 0;

        for (int i = 0; i < n * n; ++i) {
            if (!(in >> value)) {
                throw std::runtime_error("Invalid text record");
            }
        }
    }

    in.seekg(position);

    if (!in) {
        throw std::runtime_error("Failed to read record");
    }

    if (binary) {
        loaded.loadBinary(in);
    } else {
        in >> loaded;
    }

    if (!in) {
        throw std::runtime_error("Corrupted matrix record");
    }
}

// Загрузка всех матриц из файла

static Matrix* loadMatrices(
    const char* fileName,
    bool binary,
    int& count
) {
    count = 0;

    std::ifstream in(
        fileName,
        binary
            ? std::ios::in | std::ios::binary
            : std::ios::in
    );

    if (!in) {
        std::cerr
            << "Failed to open file for reading: "
            << fileName << '\n';

        count = -1;
        return nullptr;
    }

    Matrix* matrices = nullptr;
    int capacity = 0;

    try {
        while (true) {
            if (!binary) {
                in >> std::ws;
            }

            if (in.peek() == std::char_traits<char>::eof()) {
                if (in.bad() || !in.eof()) {
                    throw std::runtime_error("File read error");
                }

                break;
            }

            Matrix loaded;

            readFileMatrix(in, binary, loaded);
            appendMatrix(matrices, count, capacity, loaded);
        }

        return matrices;
    } catch (const std::exception& error) {
        delete[] matrices;

        count = -1;

        std::cerr
            << "Failed to load " << fileName
            << ": " << error.what() << '\n';

        return nullptr;
    }
}

Matrix* Matrix::loadTextMatrices(int& count) {
    return loadMatrices(textFileName, false, count);
}

Matrix* Matrix::loadBinaryMatrices(int& count) {
    return loadMatrices(binaryFileName, true, count);
}

// Загрузка конкретной записи по номеру

static bool loadMatrix(
    Matrix& matrix,
    const char* fileName,
    bool binary,
    int recordNumber
) {
    if (recordNumber <= 0) {
        std::cerr << "Record numbers start at 1.\n";
        return false;
    }

    try {
        std::ifstream in(
            fileName,
            binary
                ? std::ios::in | std::ios::binary
                : std::ios::in
        );

        if (!in) {
            throw std::runtime_error("Failed to open file");
        }

        for (int record = 0; record < recordNumber; ++record) {
            if (!binary) {
                in >> std::ws;
            }

            if (in.peek() == std::char_traits<char>::eof()) {
                if (in.bad() || !in.eof()) {
                    throw std::runtime_error("File read error");
                }

                throw std::runtime_error("Record not found");
            }

            Matrix loaded;

            readFileMatrix(in, binary, loaded);

            if (record == recordNumber - 1) {
                matrix = std::move(loaded);
            }
        }

        return true;
    } catch (const std::exception& error) {
        std::cerr
            << "Failed to load " << fileName
            << ": " << error.what() << '\n';

        return false;
    }
}

bool Matrix::loadTextMatrix(int recordNumber) {
    return loadMatrix(
        *this,
        textFileName,
        false,
        recordNumber
    );
}

bool Matrix::loadBinaryMatrix(int recordNumber) {
    return loadMatrix(
        *this,
        binaryFileName,
        true,
        recordNumber
    );
}

// Печать и освобождение массива матриц

void Matrix::printMatrices(const Matrix* matrices, int count) {
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

void Matrix::deleteMatrices(Matrix*& matrices, int& count) {
    delete[] matrices;

    matrices = nullptr;
    count = 0;
}

// Сравнение матриц

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

// Текстовый вывод

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

// Текстовый ввод

std::istream& operator>>(std::istream& in, Matrix& matrix) {
    int n = 0;

    if (!(in >> n)) {
        return in;
    }

    if (n <= 0 || n > std::numeric_limits<int>::max() / n) {
        in.setstate(std::ios::failbit);
        return in;
    }

    try {
        Matrix temporary(n);

        for (int row = 0; row < n; ++row) {
            for (int col = 0; col < n; ++col) {
                if (!(in >> temporary[row][col])) {
                    return in;
                }
            }
        }

        matrix = std::move(temporary);
    } catch (const std::invalid_argument&) {
        in.setstate(std::ios::failbit);
    } catch (const std::bad_alloc&) {
        in.setstate(std::ios::failbit);
    }

    return in;
}

// Строковое представление элементов

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