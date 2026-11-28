#include <stdexcept>
#include <gtest/gtest.h>
#include "matrix.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <limits>
#include <stdexcept>
#include "matrix_queue.h"
#include "identity_matrix.h"
#include "rectangular_matrix.h"

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
    EXPECT_TRUE(Matrix(2, data).transpose() == Matrix(2, expected));
}

TEST(MatrixTestsLab1, SummaryAddsElementwise) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {10, 20, 30, 40};
    static const int expected[4] = {11, 22, 33, 44};
    EXPECT_TRUE(Matrix(2, a).add(Matrix(2, b)) == Matrix(2, expected));
}

TEST(MatrixTestsLab1, SummaryThrowsOnDimensionMismatch) {
    Matrix a(2);
    Matrix b(3);
    EXPECT_THROW(a.add(b), std::invalid_argument);
}

TEST(MatrixTestsLab1, MatrixMultiplyComputesDotProducts) {
    static const int a[4] = {1, 2, 3, 4};
    static const int b[4] = {4, 3, 2, 1};
    static const int expected[4] = {8, 5, 20, 13};
    EXPECT_TRUE(Matrix(2, a).multiply(Matrix(2, b)) == Matrix(2, expected));
}

TEST(MatrixTestsLab1, MatrixMultiplyThrowsOnDimensionMismatch) {
    Matrix a(2);
    Matrix b(4);
    EXPECT_THROW(a.multiply(b), std::invalid_argument);
}

TEST(MatrixTestsLab1, NumberMultiplyScalesAllElements) {
    static const int data[4] = {1, 2, 3, 4};
    static const int expected[4] = {3, 6, 9, 12};
    EXPECT_TRUE(Matrix(2, data).multiplyByNumber(3) == Matrix(2, expected));
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
    char* str = m.formatElements();
    EXPECT_STREQ(str, "");
    delete[] str;
}

TEST(MatrixTestsLab1, ToString2x2Matrix) {
    static const int data[4] = {1, 2, 3, 4};
    Matrix m(2, data);
    char* str = m.formatElements();
    EXPECT_STREQ(str, "1\t2\n3\t4");
    delete[] str;
}

TEST(MatrixTestsLab1, ToStringWithNegativeNumbers) {
    static const int data[4] = {-1, -2, -3, -4};
    Matrix m(2, data);
    char* str = m.formatElements();
    EXPECT_STREQ(str, "-1\t-2\n-3\t-4");
    delete[] str;
}

TEST(MatrixTestsLab1, ToStringZeroMatrix) {
    Matrix m(2);
    char* str = m.formatElements();
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
    Matrix result = m1.add(m2);
    
    EXPECT_TRUE(result == Matrix(2, expected));
}
TEST(MatrixTestsLab1, SummaryOfPositiveAndNegativeMatrices) {
    static const int a[4] = {10, 20, 30, 40};
    static const int b[4] = {-5, -10, -15, -20};
    static const int expected[4] = {5, 10, 15, 20};
    
    Matrix m1(2, a);
    Matrix m2(2, b);
    Matrix result = m1.add(m2);
    
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

bool saveText(const Matrix* items, int count, const char* path);
bool saveBinary(const Matrix* items, int count, const char* path);
Matrix* loadText(int& count, const char* path);
Matrix* loadBinary(int& count, const char* path);
void printMatrices(std::ostream& out, const Matrix* matrices, int count);

TEST(MatrixTestsLab3, OutputOperatorWritesTwoMatrices) {
    const int data[] = {-1, 0, 3, 4};
    const int value = 7;
    std::ostringstream out;
    std::ostream& result = out << Matrix(2, data) << Matrix(1, &value);
    EXPECT_EQ(&result, &out);
    EXPECT_EQ(out.str(), "2\n-1\t0\n3\t4\n1\n7\n");
}

TEST(MatrixTestsLab3, OutputOperatorRejectsEmptyMatrix) {
    std::ostringstream out;
    out << Matrix();
    EXPECT_TRUE(out.fail());
    EXPECT_TRUE(out.str().empty());
}

TEST(MatrixTestsLab3, InputOperatorReadsTwoPreparedRecords) {
    std::istringstream in("2\n-1 0\n3 4\n\n1 7");
    Matrix first(3);
    Matrix second;
    std::istream& result = in >> first >> second;
    ASSERT_TRUE(result);
    EXPECT_EQ(&result, &in);
    const int data[] = {-1, 0, 3, 4};
    EXPECT_TRUE(first == Matrix(2, data));
    ASSERT_EQ(second.getDimension(), 1);
    EXPECT_EQ(second[0][0], 7);
    EXPECT_EQ(first(), -4);
}

TEST(MatrixTestsLab3, InputOperatorKeepsValueOnBrokenSecondRecord) {
    std::istringstream in("1 7   2 10 20 30");
    Matrix m;
    ASSERT_TRUE(in >> m);
    EXPECT_FALSE(in >> m);
    ASSERT_EQ(m.getDimension(), 1);
    EXPECT_EQ(m[0][0], 7);
}

TEST(MatrixTestsLab3, BinaryRecordLayoutAndRoundTrip) {
    const int record[] = {2, -1, 0, 3, 4};
    Matrix original(2, record + 1);
    std::stringstream stream;
    original.saveBinary(stream);
    ASSERT_EQ(stream.str(), std::string(reinterpret_cast<const char*>(record), sizeof(record)));

    Matrix restored;
    restored.loadBinary(stream);
    ASSERT_FALSE(stream.fail());
    EXPECT_TRUE(restored == original);
}

TEST(MatrixTestsLab3, PreparedFilesAndAppend) {
    const int record[] = {1, -7};
    const int data[] = {-2, 0, 3, 4};
    Matrix extra[] = {Matrix(2, data), Matrix(9)};
    extra[1][8][8] = 15;

    for (bool binary : {false, true}) {
        const char* path = binary ? "lab3_append_bin.tmp" : "lab3_append_text.tmp";
        {
            std::ofstream out(path, std::ios::binary);
            if (binary) {
                out.write(reinterpret_cast<const char*>(record), sizeof(record));
            } else {
                out << "1\r\n-7";
            }
            out.close();
            ASSERT_TRUE(out);
        }

        int count = 0;
        Matrix* items = binary ? loadBinary(count, path) : loadText(count, path);
        EXPECT_EQ(count, 1);
        EXPECT_TRUE(items && count == 1 && items[0] == Matrix(1, record + 1));
        delete[] items;

        ASSERT_TRUE(binary ? saveBinary(extra, 2, path) : saveText(extra, 2, path));
        items = binary ? loadBinary(count, path) : loadText(count, path);
        EXPECT_EQ(count, 3);
        EXPECT_TRUE(items && count == 3 && items[0] == Matrix(1, record + 1)
                    && items[1] == extra[0] && items[2] == extra[1]);
        delete[] items;
        std::remove(path);
    }
}

TEST(MatrixTestsLab3, ArrayGrowsAndKeepsOrder) {
    Matrix input[17];
    for (int i = 0; i < 17; ++i) {
        input[i] = Matrix(1, &i);
    }

    for (bool binary : {false, true}) {
        const char* path = binary ? "lab3_many_bin.tmp" : "lab3_many_text.tmp";
        std::remove(path);
        ASSERT_TRUE(binary ? saveBinary(input, 17, path) : saveText(input, 17, path));
        int count = 0;
        Matrix* items = binary ? loadBinary(count, path) : loadText(count, path);
        EXPECT_EQ(count, 17);
        EXPECT_NE(items, nullptr);
        if (items && count == 17) {
            for (int i = 0; i < count; ++i) {
                EXPECT_TRUE(items[i] == input[i]);
            }
        }
        delete[] items;
        std::remove(path);
    }
}

TEST(MatrixTestsLab3, MissingEmptyAndIncompleteFiles) {
    for (bool binary : {false, true}) {
        const char* path = binary ? "lab3_bad_bin.tmp" : "lab3_bad_text.tmp";
        std::remove(path);
        int count = 0;
        EXPECT_EQ(binary ? loadBinary(count, path) : loadText(count, path), nullptr);
        EXPECT_EQ(count, -1);
        {
            std::ofstream out(path);
            ASSERT_TRUE(out);
        }
        EXPECT_EQ(binary ? loadBinary(count, path) : loadText(count, path), nullptr);
        EXPECT_EQ(count, 0);
        {
            std::ofstream out(path, std::ios::binary);
            if (binary) {
                const int record[] = {1, 7, 2, 0};
                out.write(reinterpret_cast<const char*>(record), sizeof(record) - 1);
            } else {
                out << "1 7\n2 0";
            }
        }
        EXPECT_EQ(binary ? loadBinary(count, path) : loadText(count, path), nullptr);
        EXPECT_EQ(count, -1);
        std::remove(path);
    }
}

TEST(MatrixTestsLab3, InvalidOrIncompleteRecordKeepsOriginal) {
    const int value = -5;
    Matrix original(1, &value);
    for (int n : {0, -1, 2, 46340, std::numeric_limits<int>::max()}) {
        Matrix restored(original);
        std::stringstream text;
        text << n << " 1 x";
        EXPECT_FALSE(text >> restored);
        EXPECT_TRUE(restored == original);

        std::stringstream binary;
        binary.write(reinterpret_cast<const char*>(&n), sizeof(n));
        binary.put('x');
        restored.loadBinary(binary);
        EXPECT_TRUE(binary.fail());
        EXPECT_TRUE(restored == original);
    }
}

TEST(MatrixTestsLab3, PrintMatricesUsesOutputStream) {
    const int data[] = {-1, 0, 3, 4};
    Matrix items[] = {Matrix(2, data), Matrix(1)};
    std::ostringstream out;
    printMatrices(out, items, 2);
    EXPECT_EQ(out.str(), "Matrix 1:\n2\n-1\t0\n3\t4\n\nMatrix 2:\n1\n0\n\n");
}

TEST(MatrixTestsLab3, PreparedTextMatricesAppendToBothFiles) {
    const char* source = "lab3_example.tmp";
    const char* text = "lab3_example_text.tmp";
    const char* binary = "lab3_example_binary.tmp";
    std::remove(text);
    std::remove(binary);
    {
        std::ofstream out(source);
        out << "2\n11 -3\n0 27\n1\n-7";
        out.close();
        ASSERT_TRUE(out);
    }

    Matrix old(1);
    ASSERT_TRUE(saveText(&old, 1, text));
    ASSERT_TRUE(saveBinary(&old, 1, binary));

    int count = 0;
    Matrix* items = loadText(count, source);
    EXPECT_EQ(count, 2);
    EXPECT_TRUE(saveText(items, count, text));
    EXPECT_TRUE(saveBinary(items, count, binary));
    delete[] items;

    const int data[] = {11, -3, 0, 27};
    const int last = -7;
    for (bool useBinary : {false, true}) {
        items = useBinary ? loadBinary(count, binary) : loadText(count, text);
        EXPECT_EQ(count, 3);
        EXPECT_TRUE(items && count == 3 && items[0] == old &&
                    items[1] == Matrix(2, data) && items[2] == Matrix(1, &last));
        delete[] items;
    }
    std::remove(source);
    std::remove(text);
    std::remove(binary);
}
TEST(MatrixTestsLab5, DequeueReturnsOriginalObjectsInOrder) {
    Matrix a(1), b(2);
    MatrixQueue queue;

    EXPECT_EQ(queue.getSize(), 0);
    EXPECT_EQ(queue.dequeue(), nullptr);

    queue.enqueue(a);
    queue.enqueue(b);
    EXPECT_EQ(queue.getSize(), 2);
    EXPECT_EQ(queue.dequeue(), &a);
    queue.enqueue(a);
    EXPECT_EQ(queue.dequeue(), &b);
    EXPECT_EQ(queue.dequeue(), &a);
    EXPECT_EQ(queue.getSize(), 0);

    queue.enqueue(b);
    EXPECT_EQ(queue.dequeue(), &b);
}

TEST(MatrixTestsLab5, InsertAndRemoveAtNumberedPositions) {
    Matrix a(1), b(1), c(1), d(1);
    MatrixQueue queue;

    queue.insert(1, b);
    queue.insert(1, a);
    queue.insert(3, d);
    queue.insert(3, c);
    ASSERT_EQ(queue.getSize(), 4);

    EXPECT_EQ(queue.removeAt(2), &b);
    EXPECT_EQ(queue.removeAt(3), &d);
    queue.enqueue(d);
    EXPECT_EQ(queue.removeAt(1), &a);
    EXPECT_EQ(queue.dequeue(), &c);
    EXPECT_EQ(queue.dequeue(), &d);
    EXPECT_EQ(queue.getSize(), 0);
}

TEST(MatrixTestsLab5, InvalidPositionKeepsQueueUnchanged) {
    Matrix a(1), b(1);
    MatrixQueue queue;
    EXPECT_THROW(queue.removeAt(1), std::out_of_range);
    queue.enqueue(a);

    EXPECT_THROW(queue.insert(0, b), std::out_of_range);
    EXPECT_THROW(queue.insert(3, b), std::out_of_range);
    EXPECT_THROW(queue.removeAt(0), std::out_of_range);
    EXPECT_THROW(queue.removeAt(2), std::out_of_range);
    ASSERT_EQ(queue.getSize(), 1);
    EXPECT_EQ(queue.dequeue(), &a);
}

TEST(MatrixTestsLab5, FindUsesObjectAddressAndReturnsFirstMatch) {
    Matrix a(2), b(a), other(a);
    MatrixQueue queue;
    queue.enqueue(a);
    queue.enqueue(b);
    queue.enqueue(a);

    EXPECT_EQ(queue.find(a), 1);
    EXPECT_EQ(queue.find(b), 2);
    EXPECT_EQ(queue.find(other), 0);
    EXPECT_EQ(queue.dequeue(), &a);
    EXPECT_EQ(queue.find(a), 2);
}

TEST(MatrixTestsLab5, OutputUsesActualMatrixTypeAndCurrentValues) {
    const int data[] = {1, 2, 3, 4, 5, 6};
    Matrix square(1);
    IdentityMatrix identity(2);
    RectangularMatrix rectangle(2, 3, data);
    MatrixQueue queue;
    queue.enqueue(square);
    queue.enqueue(identity);
    queue.enqueue(rectangle);

    square[0][0] = -7;
    std::ostringstream out;
    out << queue;
    EXPECT_EQ(out.str(), "1. 1\n-7\n"
                         "2. 2\n1\t0\n0\t1\n"
                         "3. 2 3\n1\t2\t3\n4\t5\t6\n");
}
