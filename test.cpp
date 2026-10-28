#include <stdexcept>
#include <gtest/gtest.h>
#include "Matrix.h"
#include <cstring>
#include <sstream>
#include <fstream>
#include <cstdio>

TEST(MatrixTestsLab1, DefaultConstructorCreatesEmptyMatrix) {
    Matrix m;
    EXPECT_EQ(m.getDimension(), 0);
    EXPECT_TRUE(m.getElements() == nullptr);
}

TEST(MatrixTestsLab1, DimensionConstructorCreatesZeroMatrix) {
    Matrix m(3);
    ASSERT_EQ(m.getDimension(), 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            EXPECT_EQ(m.getElement(i, j), 0);
}

TEST(MatrixTestsLab1, DataConstructorCopiesElements) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    EXPECT_EQ(m.getElement(0, 0), 1);
    EXPECT_EQ(m.getElement(0, 1), 2);
    EXPECT_EQ(m.getElement(1, 0), 3);
    EXPECT_EQ(m.getElement(1, 1), 4);
}

TEST(MatrixTestsLab1, DataConstructorDoesNotAliasSourceArray) {
    int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    data[0] = 100;
    EXPECT_EQ(m.getElement(0, 0), 1);
}

TEST(MatrixTestsLab1, CopyConstructorMakesIndependentCopy) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix a(2, data);
    Matrix b(a);
    EXPECT_TRUE(b == a);
    b = b.setElement(0, 0, 99);
    EXPECT_EQ(a.getElement(0, 0), 1);
    EXPECT_EQ(b.getElement(0, 0), 99);
}

TEST(MatrixTestsLab1, MoveConstructorStealsData) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix a(2, data);
    Matrix b(std::move(a));
    ASSERT_EQ(b.getDimension(), 2);
    EXPECT_EQ(b.getElement(1, 1), 4);
    EXPECT_EQ(a.getDimension(), 0);
    EXPECT_TRUE(a.getElements() == nullptr);
}

TEST(MatrixTestsLab1, CopyAssignmentCopiesData) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {9, 8, 7, 6};
    Matrix m(2, a);
    Matrix k(2, b);
    k = m;
    EXPECT_TRUE(k == m);
}

TEST(MatrixTestsLab1, MoveAssignmentStealsData) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix a(2, data);
    Matrix b(3);
    b = std::move(a);
    ASSERT_EQ(b.getDimension(), 2);
    EXPECT_EQ(b.getElement(0, 0), 1);
    EXPECT_EQ(a.getDimension(), 0);
}

TEST(MatrixTestsLab1, SelfAssignmentIsSafeForCopyAndMove) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    Matrix* p = &m;
    *p = m;
    EXPECT_EQ(m.getElement(0, 0), 1);
    *p = std::move(m);
    EXPECT_EQ(m.getDimension(), 2);
    EXPECT_EQ(m.getElement(0, 0), 1);
}

TEST(MatrixTestsLab1, GettersReturnDimensionAndFlatData) {
    static const int data[4] = {5, 6, 7, 8};
    Matrix m(2, data);
    EXPECT_EQ(m.getDimension(), 2);
    const int* flat = m.getElements();
    ASSERT_TRUE(flat != nullptr);
    EXPECT_EQ(flat[0], 5);
    EXPECT_EQ(flat[3], 8);
}

TEST(MatrixTestsLab1, GetElementUsesRowColIndexing) {
    static const int data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix m(3, data);
    EXPECT_EQ(m.getElement(0, 2), 3);
    EXPECT_EQ(m.getElement(1, 0), 4);
    EXPECT_EQ(m.getElement(2, 1), 8);
}

TEST(MatrixTestsLab1, SetElementReturnsNewKeepsOriginal) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    Matrix changed = m.setElement(1, 0, 42);
    EXPECT_EQ(changed.getElement(1, 0), 42);
    EXPECT_EQ(m.getElement(1, 0), 3);
}

TEST(MatrixTestsLab1, SetElementsReplacesAllKeepsOriginal) {
    static const int data[4] = {1, 2, 3, 4};
    static const int fresh[4] = {9, 9, 9, 9};
    Matrix m(2, data);
    Matrix replaced = m.setElements(fresh);
    EXPECT_EQ(replaced.getElement(0, 0), 9);
    EXPECT_EQ(replaced.getElement(1, 1), 9);
    EXPECT_EQ(m.getElement(0, 0), 1);
}

TEST(MatrixTestsLab1, SetDimensionGrowsKeepsCommonBlock) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix grown = Matrix(2, data).setDimension(3);
    ASSERT_EQ(grown.getDimension(), 3);
    EXPECT_EQ(grown.getElement(1, 1), 4);
    EXPECT_EQ(grown.getElement(0, 2), 0);
    EXPECT_EQ(grown.getElement(2, 2), 0);
}

TEST(MatrixTestsLab1, SetDimensionShrinksKeepsCommonBlock) {
    static const int data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix shrunk = Matrix(3, data).setDimension(2);
    ASSERT_EQ(shrunk.getDimension(), 2);
    EXPECT_EQ(shrunk.getElement(0, 0), 1);
    EXPECT_EQ(shrunk.getElement(1, 1), 5);
}

TEST(MatrixTestsLab1, TranspositionSwapsRowsAndColumns) {
    static const int data[4] = {1, 2, 3, 4};
    static const int expected[4] = {1, 3, 2, 4};
    EXPECT_TRUE(Matrix(2, data).transposition() == Matrix(2, expected));
}

TEST(MatrixTestsLab1, SummaryAddsElementwise) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {10, 20, 30, 40};
    static const int expected[4] = {11, 22, 33, 44};
    EXPECT_TRUE(Matrix(2, a).summary(Matrix(2, b)) == Matrix(2, expected));
}

TEST(MatrixTestsLab1, SummaryThrowsOnDimensionMismatch) {
    Matrix a(2);
    Matrix b(3);
    EXPECT_THROW(a.summary(b), std::invalid_argument);
}

TEST(MatrixTestsLab1, MatrixMultiplyComputesDotProducts) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {4, 3, 2, 1};
    static const int expected[4] = {8, 5, 20, 13};
    EXPECT_TRUE(Matrix(2, a).matrixMultiply(Matrix(2, b)) == Matrix(2, expected));
}

TEST(MatrixTestsLab1, MatrixMultiplyThrowsOnDimensionMismatch) {
    Matrix a(2);
    Matrix b(4);
    EXPECT_THROW(a.matrixMultiply(b), std::invalid_argument);
}

TEST(MatrixTestsLab1, NumberMultiplyScalesAllElements) {
    static const int data[4] = {1, 2, 3, 4};
    static const int expected[4] = {3, 6, 9, 12};
    EXPECT_TRUE(Matrix(2, data).numberMultiply(3) == Matrix(2, expected));
}

TEST(MatrixTestsLab1, EqualityOperatorsCompareByValue) {
    static const int a[4] = {1, 2, 3, 4};
    static const int same[4] = {1, 2, 3, 4};
    static const int other[4] = {1, 2, 3, 5};
    Matrix m(2, a);
    EXPECT_TRUE(m == Matrix(2, same));
    EXPECT_FALSE(m == Matrix(2, other));
    EXPECT_TRUE(m != Matrix(2, other));
    EXPECT_FALSE(m == Matrix(3));
}


TEST(MatrixTestsLab1, ToStringEmptyMatrix) {
    Matrix m;
    char* str = m.toString();
    EXPECT_STREQ(str, "");
    delete[] str;
}

TEST(MatrixTestsLab1, ToString2x2Matrix) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    char* str = m.toString();
    EXPECT_STREQ(str, "1\t2\n3\t4");
    delete[] str;
}

TEST(MatrixTestsLab1, ToStringWithNegativeNumbers) {
    static const int data[4] = {-1, -2, -3, -4};
    Matrix m(2, data);
    char* str = m.toString();
    EXPECT_STREQ(str, "-1\t-2\n-3\t-4");
    delete[] str;
}

TEST(MatrixTestsLab1, ToStringZeroMatrix) {
    Matrix m(2);
    char* str = m.toString();
    EXPECT_STREQ(str, "0\t0\n0\t0");
    delete[] str;
}
TEST(MatrixTestsLab1, CreateMatrixWithNegativeNumbers) {
    static const int data[4] = {-1, -2, -3, -4};
    Matrix m(2, data);
    
    ASSERT_EQ(m.getDimension(), 2);
    
    EXPECT_EQ(m.getElement(0, 0), -1);
    EXPECT_EQ(m.getElement(0, 1), -2);
    EXPECT_EQ(m.getElement(1, 0), -3);
    EXPECT_EQ(m.getElement(1, 1), -4);
}
TEST(MatrixTestsLab1, SummaryOfTwoNegativeMatrices) {
    static const int a[4] = {-1, -2, -3, -4};
    static const int b[4] = {-5, -6, -7, -8};
    static const int expected[4] = {-6, -8, -10, -12};
    
    Matrix m1(2, a);
    Matrix m2(2, b);
    Matrix result = m1.summary(m2);
    
    EXPECT_TRUE(result == Matrix(2, expected));
}
TEST(MatrixTestsLab1, SummaryOfPositiveAndNegativeMatrices) {
    static const int a[4] = {10, 20, 30, 40};
    static const int b[4] = {-5, -10, -15, -20};
    static const int expected[4] = {5, 10, 15, 20};
    
    Matrix m1(2, a);
    Matrix m2(2, b);
    Matrix result = m1.summary(m2);
    
    EXPECT_TRUE(result == Matrix(2, expected));
}

TEST(MatrixTestsLab2, PlusAddsElementwise) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {10, 20, 30, 40};
    static const int expected[4] = {11, 22, 33, 44};
    EXPECT_TRUE(Matrix(2, a) + Matrix(2, b) == Matrix(2, expected));
}

TEST(MatrixTestsLab2, PlusWithZeroMatrixKeepsOriginal) {
    static const int a[4] = {1, 2, 3, 4};
    EXPECT_TRUE(Matrix(2, a) + Matrix(2) == Matrix(2, a));
}

TEST(MatrixTestsLab2, PlusIsCommutative) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {4, 3, 2, 1};
    EXPECT_TRUE(Matrix(2, a) + Matrix(2, b) == Matrix(2, b) + Matrix(2, a));
}

TEST(MatrixTestsLab2, PlusThrowsOnDimensionMismatch) {
    Matrix a(2);
    Matrix b(3);
    EXPECT_THROW(a + b, std::invalid_argument);
}

TEST(MatrixTestsLab2, MinusSubtractsElementwise) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {4, 3, 2, 1};
    static const int expected[4] = {-3, -1, 1, 3};
    EXPECT_TRUE(Matrix(2, a) - Matrix(2, b) == Matrix(2, expected));
}

TEST(MatrixTestsLab2, MinusSelfGivesZeroMatrix) {
    static const int a[4] = {1, 2, 3, 4};
    EXPECT_TRUE(Matrix(2, a) - Matrix(2, a) == Matrix(2));
}

TEST(MatrixTestsLab2, MinusThrowsOnDimensionMismatch) {
    Matrix a(2);
    Matrix b(3);
    EXPECT_THROW(a - b, std::invalid_argument);
}

TEST(MatrixTestsLab2, ReadsElementByRowAndColumn) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    EXPECT_EQ(m[0][1], 2);
    EXPECT_EQ(m[1][0], 3);
}

TEST(MatrixTestsLab2, WritesElementInPlace) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    m[1][1] = 9;
    EXPECT_EQ(m.getElement(1, 1), 9);
    EXPECT_EQ(m[1][1], 9);
}

TEST(MatrixTestsLab2, ConstObjectAllowsReadOnlyIndexing) {
    static const int data[4] = {1, 2, 3, 4};
    const Matrix m(2, data);
    EXPECT_EQ(m[0][0], 1);
    EXPECT_EQ(m[1][1], 4);
}

TEST(MatrixTestsLab2, WriteTouchesOnlyRequestedRow) {
    static const int data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix m(3, data);
    m[0][2] = 42;
    EXPECT_EQ(m.getElement(0, 2), 42);
    EXPECT_EQ(m.getElement(1, 2), 6);
    EXPECT_EQ(m.getElement(2, 2), 9);
}

TEST(MatrixTestsLab2, RowOutOfRangeThrows) {
    Matrix m(2);
    EXPECT_THROW(m[2], std::out_of_range);
    EXPECT_THROW(m[10], std::out_of_range);
}

TEST(MatrixTestsLab2, NegativeRowThrows) {
    Matrix m(2);
    EXPECT_THROW(m[-1], std::out_of_range);
}

TEST(MatrixTestsLab2, EmptyMatrixIndexingThrows) {
    Matrix m;
    EXPECT_THROW(m[0], std::out_of_range);
}

TEST(MatrixTestsLab2, OneByOne) {
    static const int data[1] = {5};
    EXPECT_EQ(Matrix(1, data)(), 5);
}

TEST(MatrixTestsLab2, TwoByTwo) {
    static const int data[4] = {1, 2, 3, 4};
    EXPECT_EQ(Matrix(2, data)(), -2);
}

TEST(MatrixTestsLab2, ThreeByThree) {
    static const int data[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};
    EXPECT_EQ(Matrix(3, data)(), 12);
}

TEST(MatrixTestsLab2, SingularMatrixGivesZero) {
    static const int data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(Matrix(3, data)(), 0);
}

TEST(MatrixTestsLab2, DiagonalMatrixGivesProductOfDiagonal) {
    static const int data[9] = {2, 0, 0, 0, 3, 0, 0, 0, 4};
    EXPECT_EQ(Matrix(3, data)(), 24);
}

TEST(MatrixTestsLab2, RowSwapFlipsSign) {
    static const int a[4] = {1, 2, 3, 4};
    static const int swapped[4] = {3, 4, 1, 2};
    EXPECT_EQ(Matrix(2, swapped)(), -Matrix(2, a)());
}

TEST(MatrixTestsLab2, EmptyMatrixThrows) {
    Matrix m;
    EXPECT_THROW(m(), std::invalid_argument);
}

TEST(MatrixTestsLab2, CopyAssignmentMakesIndependentCopy) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {9, 9, 9, 9};
    Matrix m(2, a);
    Matrix k(2, b);
    k = m;
    k[0][0] = 99;
    EXPECT_EQ(m.getElement(0, 0), 1);
    EXPECT_EQ(k.getElement(0, 0), 99);
}

TEST(MatrixTestsLab2, CopyAssignmentAdoptsDimension) {
    static const int a[4] = {1, 2, 3, 4};
    Matrix m(2, a);
    Matrix k(3);
    k = m;
    ASSERT_EQ(k.getDimension(), 2);
    EXPECT_TRUE(k == m);
}

TEST(MatrixTestsLab2, SelfAssignmentKeepsObjectIntact) {
    static const int a[4] = {1, 2, 3, 4};
    Matrix m(2, a);
    Matrix* p = &m;
    *p = m;
    ASSERT_EQ(m.getDimension(), 2);
    EXPECT_EQ(m.getElement(1, 1), 4);
}

TEST(MatrixTestsLab2, AssignmentReturnsReferenceToSelf) {
    static const int a[4] = {1, 2, 3, 4};
    Matrix m(2, a);
    Matrix d(2);
    Matrix& ref = (d = m);
    EXPECT_EQ(&ref, &d);
    EXPECT_TRUE(ref == m);
}

TEST(MatrixTestsLab2, MoveAssignmentLeavesSourceEmpty) {
    static const int a[4] = {1, 2, 3, 4};
    Matrix m(2, a);
    Matrix k(3);
    k = std::move(m);
    ASSERT_EQ(k.getDimension(), 2);
    EXPECT_EQ(k.getElement(0, 0), 1);
    EXPECT_EQ(m.getDimension(), 0);
}

TEST(MatrixTestsLab3, StreamOutputStartsWithDimension) {
    static const int data[4] = {1, 2, 3, 4};
    std::stringstream ss;
    ss << Matrix(2, data);
    int dimension = 0;
    ss >> dimension;
    EXPECT_EQ(dimension, 2);
}

TEST(MatrixTestsLab3, TextRoundTripSingleMatrix) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    std::stringstream ss;
    ss << m;
    Matrix r;
    ss >> r;
    EXPECT_FALSE(ss.fail());
    EXPECT_TRUE(r == m);
}

TEST(MatrixTestsLab3, TextRoundTripSeveralMatrices) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};
    std::stringstream ss;
    ss << Matrix(2, a) << Matrix(3, b);
    Matrix r1;
    Matrix r2;
    ss >> r1 >> r2;
    EXPECT_FALSE(ss.fail());
    EXPECT_TRUE(r1 == Matrix(2, a));
    EXPECT_TRUE(r2 == Matrix(3, b));
}

TEST(MatrixTestsLab3, TextRoundTripEmptyMatrix) {
    std::stringstream ss;
    ss << Matrix();
    Matrix r(2);
    ss >> r;
    EXPECT_FALSE(ss.fail());
    EXPECT_EQ(r.getDimension(), 0);
}

TEST(MatrixTestsLab3, StreamInputReadsKeyboardLikeFormat) {
    std::istringstream ss("2 5 6 7 8");
    Matrix m;
    ss >> m;
    ASSERT_EQ(m.getDimension(), 2);
    EXPECT_EQ(m.getElement(0, 0), 5);
    EXPECT_EQ(m.getElement(1, 1), 8);
}

TEST(MatrixTestsLab3, StreamInputFailsOnGarbageKeepsObject) {
    static const int data[4] = {1, 2, 3, 4};
    std::istringstream ss("oops");
    Matrix m(2, data);
    ss >> m;
    EXPECT_TRUE(ss.fail());
    EXPECT_TRUE(m == Matrix(2, data));
}

TEST(MatrixTestsLab3, StreamInputFailsOnNegativeDimension) {
    std::istringstream ss("-3 1 2");
    Matrix m(1);
    ss >> m;
    EXPECT_TRUE(ss.fail());
    EXPECT_EQ(m.getDimension(), 1);
}

TEST(MatrixTestsLab3, StreamInputFailsOnTruncatedData) {
    static const int data[4] = {1, 2, 3, 4};
    std::istringstream ss("2 1 2 3");
    Matrix m(2, data);
    ss >> m;
    EXPECT_TRUE(ss.fail());
    EXPECT_TRUE(m == Matrix(2, data));
}

TEST(MatrixTestsLab3, StreamInputLoopStopsAtEndOfStream) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};
    std::stringstream ss;
    ss << Matrix(2, a) << Matrix(3, b);
    int count = 0;
    Matrix m;
    while (ss >> m) {
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST(MatrixTestsLab3, BinaryRoundTripSingleMatrix) {
    static const int data[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};
    Matrix m(3, data);
    std::stringstream ss;
    m.saveBinary(ss);
    Matrix r;
    r.loadBinary(ss);
    EXPECT_FALSE(ss.fail());
    EXPECT_TRUE(r == m);
}

TEST(MatrixTestsLab3, BinaryRoundTripSeveralMatricesIncludingEmpty) {
    static const int a[4] = {1, 2, 3, 4};
    std::stringstream ss;
    Matrix(2, a).saveBinary(ss);
    Matrix().saveBinary(ss);
    Matrix r1;
    Matrix r2(3);
    r1.loadBinary(ss);
    r2.loadBinary(ss);
    EXPECT_FALSE(ss.fail());
    EXPECT_TRUE(r1 == Matrix(2, a));
    EXPECT_EQ(r2.getDimension(), 0);
}

TEST(MatrixTestsLab3, BinaryLoadFailsAtEndOfStream) {
    static const int a[4] = {1, 2, 3, 4};
    std::stringstream ss;
    Matrix(2, a).saveBinary(ss);
    Matrix m;
    m.loadBinary(ss);
    ASSERT_FALSE(ss.fail());
    Matrix extra;
    extra.loadBinary(ss);
    EXPECT_TRUE(ss.fail());
}

TEST(MatrixTestsLab3, TextFileSavesAndLoadsAnyNumberOfMatrices) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};
    const char* path = "matrix_lab3_text_test.txt";
    {
        std::ofstream out(path);
        out << 2 << '\n';
        out << Matrix(2, a) << Matrix(3, b);
    }
    std::ifstream in(path);
    int count = 0;
    in >> count;
    ASSERT_EQ(count, 2);
    Matrix r1;
    Matrix r2;
    in >> r1 >> r2;
    EXPECT_FALSE(in.fail());
    EXPECT_TRUE(r1 == Matrix(2, a));
    EXPECT_TRUE(r2 == Matrix(3, b));
    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryFileSavesAndLoadsAnyNumberOfMatrices) {
    static const int a[4] = {1, 2, 3, 4};
    const char* path = "matrix_lab3_bin_test.bin";
    {
        std::ofstream out(path, std::ios::binary);
        int count = 2;
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        Matrix(2, a).saveBinary(out);
        Matrix().saveBinary(out);
    }
    std::ifstream in(path, std::ios::binary);
    int count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    ASSERT_EQ(count, 2);
    Matrix r1;
    Matrix r2;
    r1.loadBinary(in);
    r2.loadBinary(in);
    EXPECT_FALSE(in.fail());
    EXPECT_TRUE(r1 == Matrix(2, a));
    EXPECT_EQ(r2.getDimension(), 0);
    std::remove(path);
}