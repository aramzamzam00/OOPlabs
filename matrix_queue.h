#pragma once
#include "matrix.h"

class MatrixQueue {
    public:
        MatrixQueue() = default;
        ~MatrixQueue();
        MatrixQueue(const MatrixQueue&) = delete;
        MatrixQueue& operator=(const MatrixQueue&) = delete;

        void enqueue(Matrix& matrix);
        Matrix* dequeue();
        void insert(int position, Matrix& matrix);
        Matrix* removeAt(int position);
        int find(const Matrix& matrix) const;
        int getSize() const;
        friend std::ostream& operator<<(std::ostream& out, const MatrixQueue& queue);
        friend std::istream& operator>>(std::istream& in, MatrixQueue& queue);

    private:
        struct Node {
            Matrix& matrix;
            Node* prev;
            Node* next;
        };

        Node* first = nullptr;
        int size = 0;
        Node* nodeAt(int position) const;
};

std::ostream& operator<<(std::ostream& out, const MatrixQueue& queue);
std::istream& operator>>(std::istream& in, MatrixQueue& queue);
void demonstrateMatrixQueue();