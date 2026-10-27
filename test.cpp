#include <stdexcept>
#include <gtest/gtest.h>
#include "Matrix.h"

#include <cstring>

TEST(MatrixTests, DefaultConstructorCreatesEmptyMatrix) {
    Matrix m;
    EXPECT_EQ(m.getDimension(), 0);
    EXPECT_TRUE(m.getElements() == nullptr);
}

TEST(MatrixTests, DimensionConstructorCreatesZeroMatrix) {
    Matrix m(3);
    ASSERT_EQ(m.getDimension(), 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            EXPECT_EQ(m.getElement(i, j), 0);
}

TEST(MatrixTests, DataConstructorCopiesElements) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    EXPECT_EQ(m.getElement(0, 0), 1);
    EXPECT_EQ(m.getElement(0, 1), 2);
    EXPECT_EQ(m.getElement(1, 0), 3);
    EXPECT_EQ(m.getElement(1, 1), 4);
}

TEST(MatrixTests, DataConstructorDoesNotAliasSourceArray) {
    int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    data[0] = 100;
    EXPECT_EQ(m.getElement(0, 0), 1);
}

TEST(MatrixTests, CopyConstructorMakesIndependentCopy) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix a(2, data);
    Matrix b(a);
    EXPECT_TRUE(b == a);
    b = b.setElement(0, 0, 99);
    EXPECT_EQ(a.getElement(0, 0), 1);
    EXPECT_EQ(b.getElement(0, 0), 99);
}

TEST(MatrixTests, MoveConstructorStealsData) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix a(2, data);
    Matrix b(std::move(a));
    ASSERT_EQ(b.getDimension(), 2);
    EXPECT_EQ(b.getElement(1, 1), 4);
    EXPECT_EQ(a.getDimension(), 0);
    EXPECT_TRUE(a.getElements() == nullptr);
}

TEST(MatrixTests, CopyAssignmentCopiesData) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {9, 8, 7, 6};
    Matrix m(2, a);
    Matrix k(2, b);
    k = m;
    EXPECT_TRUE(k == m);
}

TEST(MatrixTests, MoveAssignmentStealsData) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix a(2, data);
    Matrix b(3);
    b = std::move(a);
    ASSERT_EQ(b.getDimension(), 2);
    EXPECT_EQ(b.getElement(0, 0), 1);
    EXPECT_EQ(a.getDimension(), 0);
}

TEST(MatrixTests, SelfAssignmentIsSafeForCopyAndMove) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    Matrix* p = &m;
    *p = m;
    EXPECT_EQ(m.getElement(0, 0), 1);
    *p = std::move(m);
    EXPECT_EQ(m.getDimension(), 2);
    EXPECT_EQ(m.getElement(0, 0), 1);
}

TEST(MatrixTests, GettersReturnDimensionAndFlatData) {
    static const int data[4] = {5, 6, 7, 8};
    Matrix m(2, data);
    EXPECT_EQ(m.getDimension(), 2);
    const int* flat = m.getElements();
    ASSERT_TRUE(flat != nullptr);
    EXPECT_EQ(flat[0], 5);
    EXPECT_EQ(flat[3], 8);
}

TEST(MatrixTests, GetElementUsesRowColIndexing) {
    static const int data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix m(3, data);
    EXPECT_EQ(m.getElement(0, 2), 3);
    EXPECT_EQ(m.getElement(1, 0), 4);
    EXPECT_EQ(m.getElement(2, 1), 8);
}

TEST(MatrixTests, SetElementReturnsNewKeepsOriginal) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    Matrix changed = m.setElement(1, 0, 42);
    EXPECT_EQ(changed.getElement(1, 0), 42);
    EXPECT_EQ(m.getElement(1, 0), 3);
}

TEST(MatrixTests, SetElementsReplacesAllKeepsOriginal) {
    static const int data[4] = {1, 2, 3, 4};
    static const int fresh[4] = {9, 9, 9, 9};
    Matrix m(2, data);
    Matrix replaced = m.setElements(fresh);
    EXPECT_EQ(replaced.getElement(0, 0), 9);
    EXPECT_EQ(replaced.getElement(1, 1), 9);
    EXPECT_EQ(m.getElement(0, 0), 1);
}

TEST(MatrixTests, SetDimensionGrowsKeepsCommonBlock) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix grown = Matrix(2, data).setDimension(3);
    ASSERT_EQ(grown.getDimension(), 3);
    EXPECT_EQ(grown.getElement(1, 1), 4);
    EXPECT_EQ(grown.getElement(0, 2), 0);
    EXPECT_EQ(grown.getElement(2, 2), 0);
}

TEST(MatrixTests, SetDimensionShrinksKeepsCommonBlock) {
    static const int data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix shrunk = Matrix(3, data).setDimension(2);
    ASSERT_EQ(shrunk.getDimension(), 2);
    EXPECT_EQ(shrunk.getElement(0, 0), 1);
    EXPECT_EQ(shrunk.getElement(1, 1), 5);
}

TEST(MatrixTests, TranspositionSwapsRowsAndColumns) {
    static const int data[4] = {1, 2, 3, 4};
    static const int expected[4] = {1, 3, 2, 4};
    EXPECT_TRUE(Matrix(2, data).transposition() == Matrix(2, expected));
}

TEST(MatrixTests, SummaryAddsElementwise) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {10, 20, 30, 40};
    static const int expected[4] = {11, 22, 33, 44};
    EXPECT_TRUE(Matrix(2, a).summary(Matrix(2, b)) == Matrix(2, expected));
}

TEST(MatrixTests, SummaryThrowsOnDimensionMismatch) {
    Matrix a(2);
    Matrix b(3);
    EXPECT_THROW(a.summary(b), std::invalid_argument);
}

TEST(MatrixTests, MatrixMultiplyComputesDotProducts) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {4, 3, 2, 1};
    static const int expected[4] = {8, 5, 20, 13};
    EXPECT_TRUE(Matrix(2, a).matrixMultiply(Matrix(2, b)) == Matrix(2, expected));
}

TEST(MatrixTests, MatrixMultiplyThrowsOnDimensionMismatch) {
    Matrix a(2);
    Matrix b(4);
    EXPECT_THROW(a.matrixMultiply(b), std::invalid_argument);
}

TEST(MatrixTests, NumberMultiplyScalesAllElements) {
    static const int data[4] = {1, 2, 3, 4};
    static const int expected[4] = {3, 6, 9, 12};
    EXPECT_TRUE(Matrix(2, data).numberMultiply(3) == Matrix(2, expected));
}

TEST(MatrixTests, EqualityOperatorsCompareByValue) {
    static const int a[4] = {1, 2, 3, 4};
    static const int same[4] = {1, 2, 3, 4};
    static const int other[4] = {1, 2, 3, 5};
    Matrix m(2, a);
    EXPECT_TRUE(m == Matrix(2, same));
    EXPECT_FALSE(m == Matrix(2, other));
    EXPECT_TRUE(m != Matrix(2, other));
    EXPECT_FALSE(m == Matrix(3));
}


TEST(MatrixTests, ToStringEmptyMatrix) {
    Matrix m;
    char* str = m.toString();
    EXPECT_STREQ(str, "");
    delete[] str;
}

TEST(MatrixTests, ToString2x2Matrix) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    char* str = m.toString();
    EXPECT_STREQ(str, "1\t2\n3\t4");
    delete[] str;
}

TEST(MatrixTests, ToStringWithNegativeNumbers) {
    static const int data[4] = {-1, -2, -3, -4};
    Matrix m(2, data);
    char* str = m.toString();
    EXPECT_STREQ(str, "-1\t-2\n-3\t-4");
    delete[] str;
}

TEST(MatrixTests, ToStringZeroMatrix) {
    Matrix m(2);
    char* str = m.toString();
    EXPECT_STREQ(str, "0\t0\n0\t0");
    delete[] str;
}
TEST(MatrixTests, CreateMatrixWithNegativeNumbers) {
    static const int data[4] = {-1, -2, -3, -4};
    Matrix m(2, data);
    
    ASSERT_EQ(m.getDimension(), 2);
    
    EXPECT_EQ(m.getElement(0, 0), -1);
    EXPECT_EQ(m.getElement(0, 1), -2);
    EXPECT_EQ(m.getElement(1, 0), -3);
    EXPECT_EQ(m.getElement(1, 1), -4);
}
TEST(MatrixTests, SummaryOfTwoNegativeMatrices) {
    static const int a[4] = {-1, -2, -3, -4};
    static const int b[4] = {-5, -6, -7, -8};
    static const int expected[4] = {-6, -8, -10, -12};
    
    Matrix m1(2, a);
    Matrix m2(2, b);
    Matrix result = m1.summary(m2);
    
    EXPECT_TRUE(result == Matrix(2, expected));
}
TEST(MatrixTests, SummaryOfPositiveAndNegativeMatrices) {
    static const int a[4] = {10, 20, 30, 40};
    static const int b[4] = {-5, -10, -15, -20};
    static const int expected[4] = {5, 10, 15, 20};
    
    Matrix m1(2, a);
    Matrix m2(2, b);
    Matrix result = m1.summary(m2);
    
    EXPECT_TRUE(result == Matrix(2, expected));
}