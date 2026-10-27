#pragma once
#include <iosfwd>  
class Matrix {
    public:
        // Конструкторы
        Matrix();
        explicit Matrix(int newDimension);
        Matrix(int newDimension, const int* newElements);

        // Правило пяти
        ~Matrix();
        Matrix(const Matrix& other);
        Matrix(Matrix&& other);
        Matrix& operator=(const Matrix& other);
        Matrix& operator=(Matrix&& other);

        // геттеры сеттеры
        int getDimension() const; 
        Matrix setDimension(int newDimension) const;

        const int* getElements() const;
        Matrix setElements(const int* newElements) const;

        int getElement(int row, int col) const;
        Matrix setElement(int row, int col, int value) const;

        // операции
        Matrix transposition() const;
        Matrix summary(const Matrix& other) const;
        Matrix matrixMultiply(const Matrix& other) const;
        Matrix numberMultiply(int number) const;
        char* toString() const;

    private:
        int dimension;
        int* elements;
    
};
bool operator==(const Matrix& leftMatrix, const Matrix& rightMatrix);
bool operator!=(const Matrix& leftMatrix, const Matrix& rightMatrix);
std::ostream& operator<<(std::ostream& ostream, const Matrix& matrix);