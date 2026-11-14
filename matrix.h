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
        Matrix transpose() const;
        Matrix add(const Matrix& other) const;
        Matrix multiply(const Matrix& other) const;
        Matrix multiplyByNumber(int number) const;
        char* formatElements() const;

        //Лаба 2
        Matrix operator+(const Matrix& other) const;
        friend Matrix operator-(const Matrix& leftMatrix, const Matrix& rightMatrix);
        
        int* operator[](int row);
        const int* operator[](int row) const;
        
        int operator()() const;

        // Лабораторная 3
        void saveBinary(std::ostream& out) const;
        void loadBinary(std::istream& in);

        bool readMatrix();

        bool saveTextMatrix(const char* fileName = nullptr) const;
        bool saveBinaryMatrix(const char* fileName = nullptr) const;

        bool loadTextMatrix(int recordNumber, const char* fileName = nullptr);
        bool loadBinaryMatrix(int recordNumber, const char* fileName = nullptr);

        static Matrix* loadTextMatrices(int& count, const char* fileName = nullptr);
        static Matrix* loadBinaryMatrices(int& count, const char* fileName = nullptr);

        static void printMatrices(const Matrix* matrices, int count);
        static void deleteMatrices(Matrix*& matrices, int& count);


    private:
        int dimension;
        int* elements;
    
};
bool operator==(const Matrix& leftMatrix, const Matrix& rightMatrix);
bool operator!=(const Matrix& leftMatrix, const Matrix& rightMatrix);
std::ostream& operator<<(std::ostream& ostream, const Matrix& matrix);
std::istream& operator>>(std::istream& istream, Matrix& matrix);