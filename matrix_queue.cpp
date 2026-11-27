#include "matrix_queue.h"
#include "identity_matrix.h"
#include "rectangular_matrix.h"
#include <iostream>
#include <stdexcept>

MatrixQueue::~MatrixQueue() {
    while (first) {
        dequeue();
    }
}

int MatrixQueue::getSize() const {
    return size;
}

MatrixQueue::Node* MatrixQueue::nodeAt(int position) const {
    if (position < 1 || position > size) {
        throw std::out_of_range("Invalid queue position");
    }
    Node* current = first;
    for (int i = 1; i < position; ++i) {
        current = current->next;
    }
    return current;
}

void MatrixQueue::enqueue(Matrix& matrix) {
    insert(size + 1, matrix);
}

void MatrixQueue::insert(int position, Matrix& matrix) {
    if (position < 1 || position > size + 1) {
        throw std::out_of_range("Invalid queue position");
    }

    Node* next = position == size + 1 ? first : nodeAt(position);
    Node* node = new Node{matrix, nullptr, nullptr};
    if (next) {
        node->prev = next->prev;
        node->next = next;
        next->prev->next = node;
        next->prev = node;
    } else {
        node->prev = node;
        node->next = node;
    }
    if (position == 1) {
        first = node;
    }
    ++size;
}

Matrix* MatrixQueue::dequeue() {
    return first ? removeAt(1) : nullptr;
}

Matrix* MatrixQueue::removeAt(int position) {
    Node* node = nodeAt(position);
    Matrix* matrix = &node->matrix;
    if (size == 1) {
        first = nullptr;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (node == first) {
            first = node->next;
        }
    }
    delete node;
    --size;
    return matrix;
}

int MatrixQueue::find(const Matrix& matrix) const {
    Node* current = first;
    for (int i = 1; i <= size; ++i) {
        if (&current->matrix == &matrix) {
            return i;
        }
        current = current->next;
    }
    return 0;
}

std::ostream& operator<<(std::ostream& out, const MatrixQueue& queue) {
    const MatrixQueue::Node* current = queue.first;
    for (int i = 1; i <= queue.size && out; ++i) {
        out << i << ". " << current->matrix;
        current = current->next;
    }
    return out;
}

std::istream& operator>>(std::istream& in, MatrixQueue& queue) {
    MatrixQueue::Node* current = queue.first;
    for (int i = 1; i <= queue.size; ++i) {
        int number;
        char dot;
        if (!(in >> number >> dot) || number != i || dot != '.') {
            in.setstate(std::ios::failbit);
            break;
        }
        if (!(in >> current->matrix)) {
            break;
        }
        current = current->next;
    }
    return in;
}

void demonstrateMatrixQueue() {
    const int data[] = {1, 2, 3, 4, 5, 6};
    Matrix square(2, data);
    IdentityMatrix identity(3);
    RectangularMatrix rectangle(2, 3, data);
    MatrixQueue queue;

    queue.enqueue(square);
    queue.enqueue(rectangle);
    queue.insert(2, identity);
    std::cout << queue;
    std::cout << "Identity position: " << queue.find(identity) << '\n';

    queue.removeAt(2);
    queue.dequeue();
    std::cout << "After removal:\n";
    std::cout << queue;
}
