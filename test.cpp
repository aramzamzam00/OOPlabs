#include <stdexcept>
#include <gtest/gtest.h>
#include "matrix.h"
#include <cstring>
#include <filesystem>
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
TEST(MatrixTestsLab3, TextLoadsExistingFileWithoutSavingAndPreservesBytes) {
    const char* path = "lab3_text_file.tmp";
    static const int data[4] = {-1, 0, 3, -4};

    // Создаём файл без вызова методов сохранения Matrix.
    {
        std::ofstream out(path);
        out << "2\n-1 0\n3 -4";
        ASSERT_FALSE(out.fail());
    }

    std::string before;
    {
        std::ifstream in(path, std::ios::binary);
        before.assign(std::istreambuf_iterator<char>(in), {});
    }

    Matrix restored(3);

    ASSERT_TRUE(restored.loadTextMatrix(1, path));
    ASSERT_EQ(restored.getDimension(), 2);
    EXPECT_TRUE(restored == Matrix(2, data));

    EXPECT_TRUE(
        restored.multiplyByNumber(2) ==
        Matrix(2, data).multiplyByNumber(2)
    );

    EXPECT_EQ(restored(), 4);

    int count = 0;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_EQ(count, 1);

    if (matrices && count == 1) {
        EXPECT_TRUE(matrices[0] == restored);
    }

    Matrix::deleteMatrices(matrices, count);

    {
        std::ifstream in(path, std::ios::binary);
        std::string after(std::istreambuf_iterator<char>(in), {});

        EXPECT_EQ(after, before);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, TextMethodsAppendDifferentOrdersAndLoadByNumber) {
    const char* path = "lab3_text_file.tmp";
    static const int data[4] = {1, 2, 3, 4};
    static const int data3[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};

    Matrix a(2, data);
    Matrix b(3, data3);

    {
        std::ofstream out(path);
        ASSERT_TRUE(out.is_open());
    }

    ASSERT_TRUE(a.saveTextMatrix(path));

    std::string prefix;
    {
        std::ifstream in(path, std::ios::binary);
        prefix.assign(std::istreambuf_iterator<char>(in), {});
    }

    ASSERT_TRUE(b.saveTextMatrix(path));

    {
        std::ifstream in(path, std::ios::binary);
        std::string contents(std::istreambuf_iterator<char>(in), {});

        EXPECT_EQ(contents.substr(0, prefix.size()), prefix);
    }

    Matrix restored;

    ASSERT_TRUE(restored.loadTextMatrix(2, path));
    EXPECT_TRUE(restored == b);

    ASSERT_TRUE(restored.loadTextMatrix(1, path));
    EXPECT_TRUE(restored == a);

    int count = 0;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_EQ(count, 2);

    if (matrices && count == 2) {
        EXPECT_TRUE(matrices[0] == a);
        EXPECT_TRUE(matrices[1] == b);
    }

    Matrix::deleteMatrices(matrices, count);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, 0);

    std::remove(path);
}

TEST(MatrixTestsLab3, TextMethodsLoadManyMatricesInOrder) {
    const char* path = "lab3_text_file.tmp";

    {
        std::ofstream out(path);
        ASSERT_TRUE(out.is_open());
    }

    for (int i = 0; i < 25; ++i) {
        Matrix original(1, &i);
        ASSERT_TRUE(original.saveTextMatrix(path));
    }

    int count = 0;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_EQ(count, 25);

    if (matrices && count == 25) {
        for (int i = 0; i < count; ++i) {
            EXPECT_EQ(matrices[i].getElement(0, 0), i);
        }
    }

    Matrix::deleteMatrices(matrices, count);

    std::remove(path);
}

TEST(MatrixTestsLab3, TextMethodsReportMissingFileWithoutCreatingIt) {
    const char* path = "lab3_text_file.tmp";

    {
        std::ofstream out(path);
        ASSERT_TRUE(out.is_open());
    }

    ASSERT_EQ(std::remove(path), 0);

    Matrix restored(2);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadTextMatrix(1, path));
    EXPECT_TRUE(restored == before);

    int count = 42;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, -1);

    Matrix::deleteMatrices(matrices, count);

    {
        std::ifstream in(path);
        EXPECT_FALSE(in.is_open());
    }
}

TEST(MatrixTestsLab3, TextMethodsTreatEmptyFileAsZeroRecords) {
    const char* path = "lab3_text_file.tmp";

    {
        std::ofstream out(path);
        ASSERT_TRUE(out.is_open());
    }

    int count = 42;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, 0);

    Matrix::deleteMatrices(matrices, count);

    Matrix restored(2);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadTextMatrix(1, path));
    EXPECT_TRUE(restored == before);

    {
        std::ifstream in(path);

        EXPECT_EQ(in.peek(), std::char_traits<char>::eof());
        EXPECT_FALSE(in.bad());
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, TextInvalidRecordNumbersKeepObjectAndFileUnchanged) {
    const char* path = "lab3_text_file.tmp";

    {
        std::ofstream out(path);
        ASSERT_TRUE(out.is_open());
    }

    Matrix original(2);
    ASSERT_TRUE(original.saveTextMatrix(path));

    std::string beforeFile;
    {
        std::ifstream in(path, std::ios::binary);
        beforeFile.assign(std::istreambuf_iterator<char>(in), {});
    }

    for (int record : {-1, 0, 2, std::numeric_limits<int>::max()}) {
        Matrix restored(3);
        Matrix before(restored);

        EXPECT_FALSE(restored.loadTextMatrix(record, path));
        EXPECT_TRUE(restored == before);
    }

    {
        std::ifstream in(path, std::ios::binary);
        std::string after(std::istreambuf_iterator<char>(in), {});

        EXPECT_EQ(after, beforeFile);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, TextFileRejectsInvalidDimensions) {
    const char* path = "lab3_text_file.tmp";

    for (int dimension : {-2, 0, 46341, std::numeric_limits<int>::max()}) {
        {
            std::ofstream out(path);
            out << dimension << '\n';
            ASSERT_FALSE(out.fail());
        }

        Matrix restored(2);
        Matrix before(restored);

        EXPECT_FALSE(restored.loadTextMatrix(1, path));
        EXPECT_TRUE(restored == before);

        int count = 0;
        Matrix* matrices = Matrix::loadTextMatrices(count, path);

        EXPECT_TRUE(matrices == nullptr);
        EXPECT_EQ(count, -1);

        Matrix::deleteMatrices(matrices, count);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, TextLargeIncompleteRecordIsRejectedBeforeAllocation) {
    const char* path = "lab3_text_file.tmp";
    const int dimension = 46340;

    {
        std::ofstream out(path);
        out << dimension << '\n';
        ASSERT_FALSE(out.fail());
    }

    Matrix restored(1);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadTextMatrix(1, path));
    EXPECT_TRUE(restored == before);

    int count = 0;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, -1);

    Matrix::deleteMatrices(matrices, count);

    std::remove(path);
}

TEST(MatrixTestsLab3, TextFileMethodsRejectTruncatedHeadersAndMissingElements) {
    const char* path = "lab3_text_file.tmp";
    const std::string record = "2\n111 -222 333 -444\n";

    // Проверяем префиксы до знака последнего числа включительно.
    // Обрыв внутри числа может оставить другое допустимое целое.
    for (std::size_t length = 1;
         length <= record.rfind('-') + 1;
         ++length) {
        {
            std::ofstream out(path);

            out.write(
                record.data(),
                static_cast<std::streamsize>(length)
            );

            ASSERT_FALSE(out.fail());
        }

        Matrix restored(3);
        Matrix before(restored);

        EXPECT_FALSE(restored.loadTextMatrix(1, path)) << length;
        EXPECT_TRUE(restored == before);

        int count = 0;
        Matrix* matrices = Matrix::loadTextMatrices(count, path);

        EXPECT_TRUE(matrices == nullptr);
        EXPECT_EQ(count, -1);

        Matrix::deleteMatrices(matrices, count);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, TextCorruptLaterRecordDoesNotReturnPartialArray) {
    const char* path = "lab3_text_file.tmp";

    {
        std::ofstream out(path);
        ASSERT_TRUE(out.is_open());
    }

    Matrix original(2);
    ASSERT_TRUE(original.saveTextMatrix(path));

    {
        std::ofstream out(path, std::ios::app);
        out << "garbage";
        ASSERT_FALSE(out.fail());
    }

    std::string beforeFile;
    {
        std::ifstream in(path, std::ios::binary);
        beforeFile.assign(std::istreambuf_iterator<char>(in), {});
    }

    int count = 0;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, -1);

    Matrix::deleteMatrices(matrices, count);

    Matrix restored(3);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadTextMatrix(2, path));
    EXPECT_TRUE(restored == before);

    {
        std::ifstream in(path, std::ios::binary);
        std::string after(std::istreambuf_iterator<char>(in), {});

        EXPECT_EQ(after, beforeFile);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, TextEmptyMatrixIsNotSavedAndWriteFailureIsReported) {
    const char* path = "lab3_text_file.tmp";

    std::remove(path);

    Matrix empty;
    EXPECT_FALSE(empty.saveTextMatrix(path));

    {
        std::ifstream in(path);
        EXPECT_FALSE(in.is_open());
    }

    // Каталог с именем файла нельзя открыть для записи как обычный файл.
    ASSERT_TRUE(std::filesystem::create_directory(path));

    Matrix original(2);
    EXPECT_FALSE(original.saveTextMatrix(path));

    EXPECT_TRUE(std::filesystem::remove(path));
}

TEST(MatrixTestsLab3, TextDirectoryCannotBeLoadedAsMatrixFile) {
    const char* path = "lab3_text_file.tmp";

    std::remove(path);

    ASSERT_TRUE(std::filesystem::create_directory(path));

    Matrix restored(2);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadTextMatrix(1, path));
    EXPECT_TRUE(restored == before);

    int count = 0;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, -1);

    Matrix::deleteMatrices(matrices, count);

    EXPECT_TRUE(std::filesystem::remove(path));
}

TEST(MatrixTestsLab3, TextAppendsAfterPreexistingRecordWithoutTrailingWhitespace) {
    const char* path = "lab3_text_file.tmp";

    {
        std::ofstream out(path);
        out << "1 7";
        ASSERT_FALSE(out.fail());
    }

    const int value = 9;
    Matrix original(1, &value);

    ASSERT_TRUE(original.saveTextMatrix(path));

    int count = 0;
    Matrix* matrices = Matrix::loadTextMatrices(count, path);

    EXPECT_EQ(count, 2);

    if (matrices && count == 2) {
        EXPECT_EQ(matrices[0].getElement(0, 0), 7);
        EXPECT_EQ(matrices[1].getElement(0, 0), 9);
    }

    Matrix::deleteMatrices(matrices, count);

    std::remove(path);
}

// Двоичный файл

TEST(MatrixTestsLab3, BinaryLoadsExistingFileWithoutSavingAndPreservesBytes) {
    const char* path = "lab3_binary_file.tmp";
    static const int data[4] = {-1, 0, 3, -4};
    static const int record[5] = {2, -1, 0, 3, -4};

    // Создаём файл без вызова методов сохранения Matrix.
    {
        std::ofstream out(path, std::ios::binary);

        out.write(
            reinterpret_cast<const char*>(record),
            sizeof(record)
        );

        ASSERT_FALSE(out.fail());
    }

    std::string before;
    {
        std::ifstream in(path, std::ios::binary);
        before.assign(std::istreambuf_iterator<char>(in), {});
    }

    Matrix restored(3);

    ASSERT_TRUE(restored.loadBinaryMatrix(1, path));
    ASSERT_EQ(restored.getDimension(), 2);
    EXPECT_TRUE(restored == Matrix(2, data));

    EXPECT_TRUE(
        restored.multiplyByNumber(2) ==
        Matrix(2, data).multiplyByNumber(2)
    );

    EXPECT_EQ(restored(), 4);

    int count = 0;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_EQ(count, 1);

    if (matrices && count == 1) {
        EXPECT_TRUE(matrices[0] == restored);
    }

    Matrix::deleteMatrices(matrices, count);

    {
        std::ifstream in(path, std::ios::binary);
        std::string after(std::istreambuf_iterator<char>(in), {});

        EXPECT_EQ(after, before);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryMethodsAppendDifferentOrdersAndLoadByNumber) {
    const char* path = "lab3_binary_file.tmp";
    static const int data[4] = {1, 2, 3, 4};
    static const int data3[9] = {2, 0, 1, 1, 3, 2, 3, 1, 4};

    Matrix a(2, data);
    Matrix b(3, data3);

    {
        std::ofstream out(path, std::ios::binary);
        ASSERT_TRUE(out.is_open());
    }

    ASSERT_TRUE(a.saveBinaryMatrix(path));

    std::string prefix;
    {
        std::ifstream in(path, std::ios::binary);
        prefix.assign(std::istreambuf_iterator<char>(in), {});
    }

    ASSERT_TRUE(b.saveBinaryMatrix(path));

    {
        std::ifstream in(path, std::ios::binary);
        std::string contents(std::istreambuf_iterator<char>(in), {});

        EXPECT_EQ(contents.substr(0, prefix.size()), prefix);
    }

    Matrix restored;

    ASSERT_TRUE(restored.loadBinaryMatrix(2, path));
    EXPECT_TRUE(restored == b);

    ASSERT_TRUE(restored.loadBinaryMatrix(1, path));
    EXPECT_TRUE(restored == a);

    int count = 0;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_EQ(count, 2);

    if (matrices && count == 2) {
        EXPECT_TRUE(matrices[0] == a);
        EXPECT_TRUE(matrices[1] == b);
    }

    Matrix::deleteMatrices(matrices, count);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, 0);

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryMethodsLoadManyMatricesInOrder) {
    const char* path = "lab3_binary_file.tmp";

    {
        std::ofstream out(path, std::ios::binary);
        ASSERT_TRUE(out.is_open());
    }

    for (int i = 0; i < 25; ++i) {
        Matrix original(1, &i);
        ASSERT_TRUE(original.saveBinaryMatrix(path));
    }

    int count = 0;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_EQ(count, 25);

    if (matrices && count == 25) {
        for (int i = 0; i < count; ++i) {
            EXPECT_EQ(matrices[i].getElement(0, 0), i);
        }
    }

    Matrix::deleteMatrices(matrices, count);

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryMethodsReportMissingFileWithoutCreatingIt) {
    const char* path = "lab3_binary_file.tmp";

    {
        std::ofstream out(path, std::ios::binary);
        ASSERT_TRUE(out.is_open());
    }

    ASSERT_EQ(std::remove(path), 0);

    Matrix restored(2);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadBinaryMatrix(1, path));
    EXPECT_TRUE(restored == before);

    int count = 42;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, -1);

    Matrix::deleteMatrices(matrices, count);

    {
        std::ifstream in(path, std::ios::binary);
        EXPECT_FALSE(in.is_open());
    }
}

TEST(MatrixTestsLab3, BinaryMethodsTreatEmptyFileAsZeroRecords) {
    const char* path = "lab3_binary_file.tmp";

    {
        std::ofstream out(path, std::ios::binary);
        ASSERT_TRUE(out.is_open());
    }

    int count = 42;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, 0);

    Matrix::deleteMatrices(matrices, count);

    Matrix restored(2);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadBinaryMatrix(1, path));
    EXPECT_TRUE(restored == before);

    {
        std::ifstream in(path, std::ios::binary);

        EXPECT_EQ(in.peek(), std::char_traits<char>::eof());
        EXPECT_FALSE(in.bad());
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryInvalidRecordNumbersKeepObjectAndFileUnchanged) {
    const char* path = "lab3_binary_file.tmp";

    {
        std::ofstream out(path, std::ios::binary);
        ASSERT_TRUE(out.is_open());
    }

    Matrix original(2);
    ASSERT_TRUE(original.saveBinaryMatrix(path));

    std::string beforeFile;
    {
        std::ifstream in(path, std::ios::binary);
        beforeFile.assign(std::istreambuf_iterator<char>(in), {});
    }

    for (int record : {-1, 0, 2, std::numeric_limits<int>::max()}) {
        Matrix restored(3);
        Matrix before(restored);

        EXPECT_FALSE(restored.loadBinaryMatrix(record, path));
        EXPECT_TRUE(restored == before);
    }

    {
        std::ifstream in(path, std::ios::binary);
        std::string after(std::istreambuf_iterator<char>(in), {});

        EXPECT_EQ(after, beforeFile);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryFileRejectsInvalidDimensions) {
    const char* path = "lab3_binary_file.tmp";

    for (int dimension : {-2, 0, 46341, std::numeric_limits<int>::max()}) {
        {
            std::ofstream out(path, std::ios::binary);

            out.write(
                reinterpret_cast<const char*>(&dimension),
                sizeof(dimension)
            );

            ASSERT_FALSE(out.fail());
        }

        Matrix restored(2);
        Matrix before(restored);

        EXPECT_FALSE(restored.loadBinaryMatrix(1, path));
        EXPECT_TRUE(restored == before);

        int count = 0;
        Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

        EXPECT_TRUE(matrices == nullptr);
        EXPECT_EQ(count, -1);

        Matrix::deleteMatrices(matrices, count);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryLargeIncompleteRecordIsRejectedBeforeAllocation) {
    const char* path = "lab3_binary_file.tmp";
    const int dimension = 46340;

    {
        std::ofstream out(path, std::ios::binary);

        out.write(
            reinterpret_cast<const char*>(&dimension),
            sizeof(dimension)
        );

        ASSERT_FALSE(out.fail());
    }

    Matrix restored(1);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadBinaryMatrix(1, path));
    EXPECT_TRUE(restored == before);

    int count = 0;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, -1);

    Matrix::deleteMatrices(matrices, count);

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryFileMethodsRejectEveryTruncatedRecord) {
    const char* path = "lab3_binary_file.tmp";
    static const int record[5] = {2, 111, -222, 333, -444};

    for (int bytes = 1; bytes < static_cast<int>(sizeof(record)); ++bytes) {
        {
            std::ofstream out(path, std::ios::binary);

            out.write(
                reinterpret_cast<const char*>(record),
                bytes
            );

            ASSERT_FALSE(out.fail());
        }

        Matrix restored(3);
        Matrix before(restored);

        EXPECT_FALSE(restored.loadBinaryMatrix(1, path)) << bytes;
        EXPECT_TRUE(restored == before);

        int count = 0;
        Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

        EXPECT_TRUE(matrices == nullptr);
        EXPECT_EQ(count, -1);

        Matrix::deleteMatrices(matrices, count);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryCorruptLaterRecordDoesNotReturnPartialArray) {
    const char* path = "lab3_binary_file.tmp";

    {
        std::ofstream out(path, std::ios::binary);
        ASSERT_TRUE(out.is_open());
    }

    Matrix original(2);
    ASSERT_TRUE(original.saveBinaryMatrix(path));

    {
        std::ofstream out(
            path,
            std::ios::binary | std::ios::app
        );

        out.put('\x7f');
        ASSERT_FALSE(out.fail());
    }

    std::string beforeFile;
    {
        std::ifstream in(path, std::ios::binary);
        beforeFile.assign(std::istreambuf_iterator<char>(in), {});
    }

    int count = 0;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, -1);

    Matrix::deleteMatrices(matrices, count);

    Matrix restored(3);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadBinaryMatrix(2, path));
    EXPECT_TRUE(restored == before);

    {
        std::ifstream in(path, std::ios::binary);
        std::string after(std::istreambuf_iterator<char>(in), {});

        EXPECT_EQ(after, beforeFile);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, BinaryEmptyMatrixIsNotSavedAndWriteFailureIsReported) {
    const char* path = "lab3_binary_file.tmp";

    std::remove(path);

    Matrix empty;
    EXPECT_FALSE(empty.saveBinaryMatrix(path));

    {
        std::ifstream in(path, std::ios::binary);
        EXPECT_FALSE(in.is_open());
    }

    // Каталог с именем файла нельзя открыть для записи как обычный файл.
    ASSERT_TRUE(std::filesystem::create_directory(path));

    Matrix original(2);
    EXPECT_FALSE(original.saveBinaryMatrix(path));

    EXPECT_TRUE(std::filesystem::remove(path));
}

TEST(MatrixTestsLab3, BinaryDirectoryCannotBeLoadedAsMatrixFile) {
    const char* path = "lab3_binary_file.tmp";

    std::remove(path);

    ASSERT_TRUE(std::filesystem::create_directory(path));

    Matrix restored(2);
    Matrix before(restored);

    EXPECT_FALSE(restored.loadBinaryMatrix(1, path));
    EXPECT_TRUE(restored == before);

    int count = 0;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_TRUE(matrices == nullptr);
    EXPECT_EQ(count, -1);

    Matrix::deleteMatrices(matrices, count);

    EXPECT_TRUE(std::filesystem::remove(path));
}

TEST(MatrixTestsLab3, BinaryAppendsAfterPreexistingRecordWithoutTrailingWhitespace) {
    const char* path = "lab3_binary_file.tmp";

    {
        std::ofstream out(path, std::ios::binary);
        static const int record[2] = {1, 7};

        out.write(
            reinterpret_cast<const char*>(record),
            sizeof(record)
        );

        ASSERT_FALSE(out.fail());
    }

    const int value = 9;
    Matrix original(1, &value);

    ASSERT_TRUE(original.saveBinaryMatrix(path));

    int count = 0;
    Matrix* matrices = Matrix::loadBinaryMatrices(count, path);

    EXPECT_EQ(count, 2);

    if (matrices && count == 2) {
        EXPECT_EQ(matrices[0].getElement(0, 0), 7);
        EXPECT_EQ(matrices[1].getElement(0, 0), 9);
    }

    Matrix::deleteMatrices(matrices, count);

    std::remove(path);
}

// Проверка некорректного ввода и конструкторов

TEST(MatrixTestsLab3, StreamInputRejectsInvalidDimensionsAndKeepsOldValue) {
    const char* path = "lab3_invalid_dimension.tmp";

    for (int dimension : {-2, 0, 46341, std::numeric_limits<int>::max()}) {
        {
            std::ofstream out(path);
            out << dimension;
            ASSERT_FALSE(out.fail());
        }

        Matrix restored(2);
        Matrix before(restored);

        {
            std::ifstream in(path);
            in >> restored;
            EXPECT_TRUE(in.fail());
        }

        EXPECT_TRUE(restored == before);

        {
            std::ofstream out(path, std::ios::binary);

            out.write(
                reinterpret_cast<const char*>(&dimension),
                sizeof(dimension)
            );

            ASSERT_FALSE(out.fail());
        }

        {
            std::ifstream in(path, std::ios::binary);
            restored.loadBinary(in);
            EXPECT_TRUE(in.fail());
        }

        EXPECT_TRUE(restored == before);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, InvalidElementsAndIncompleteInputKeepOldValue) {
    const char* path = "lab3_invalid_elements.tmp";

    for (const char* input : {
        "2 1 2 3",
        "2 1 x 3 4",
        "2 1 2 3 2147483648"
    }) {
        {
            std::ofstream out(path);
            out << input;
            ASSERT_FALSE(out.fail());
        }

        Matrix restored(3);
        Matrix before(restored);

        {
            std::ifstream in(path);
            in >> restored;
            EXPECT_TRUE(in.fail());
        }

        EXPECT_TRUE(restored == before);
    }

    std::remove(path);
}

TEST(MatrixTestsLab3, ConstructorsRejectInvalidDimensionsAndNullData) {
    EXPECT_THROW(Matrix(-1), std::invalid_argument);
    EXPECT_THROW(Matrix(46341), std::invalid_argument);
    EXPECT_THROW(Matrix(2, nullptr), std::invalid_argument);

    EXPECT_EQ(Matrix(0).getDimension(), 0);
}