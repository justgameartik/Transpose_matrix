#include <gtest/gtest.h>
#include <random>
#include "worker.hpp"

std::mt19937 gen(123512);

std::vector<Matrix> GenerateMatrixes(size_t num_of_matrix=5){
    std::vector<Matrix> mtxs(num_of_matrix);
    for (size_t mtx_num = 0; mtx_num < num_of_matrix; mtx_num++) {
        int num_of_values = gen() % 100;
        mtxs[mtx_num].data.resize(num_of_values);
        for (size_t value_num = 0; value_num < num_of_values; value_num++) {
            mtxs[mtx_num].data[value_num] = gen() % 1000;
        }
        mtxs[mtx_num].width = (gen() % num_of_values)+1;
        mtxs[mtx_num].height = num_of_values - mtxs[mtx_num].width;
    } 

    return mtxs;
}

Matrix MakeTranspose(const Matrix& matrix) {
    Matrix transposed_matrix {
        std::vector<int>(matrix.data.size()),
        matrix.height,
        matrix.width
    };

    for (size_t i = 0; i < matrix.height; i++) {
        for (size_t j = 0; j < matrix.width; j++) {
            transposed_matrix.data[i+j*transposed_matrix.width] =
                matrix.data[i*matrix.width+j];
        }
    }
    
    return transposed_matrix;
}

Matrix SumMatrixes(Matrix mtx1, Matrix mtx2) {
    if (mtx1.height != mtx2.height || mtx1.width != mtx2.width) {
        return Matrix {
            std::vector<int> (0),
            0,
            0
        };
    }

    Matrix sum {
        std::vector<int> (mtx1.height+mtx1.width),
        mtx1.height,
        mtx1.width
    };

    for (size_t i = 0; i < mtx1.height+mtx1.width; i++) {
        sum.data[i] = mtx1.data[i] + mtx2.data[i];
    }

    return sum;
}

TEST(Features, DoubleTranspose) {
    for (size_t i = 0; i < 1000; i++) {
        auto mtx = GenerateMatrixes(1)[0];
        auto double_transposed_mtx = MakeTranspose(MakeTranspose(mtx));
        EXPECT_EQ(mtx.data, double_transposed_mtx.data);
        EXPECT_EQ(mtx.height, double_transposed_mtx.height);
        EXPECT_EQ(mtx.width, double_transposed_mtx.width);
    }
}

TEST(Features, SumOfMtxs) {
    for (size_t i = 0; i < 1000; i++) {
        auto mtxs = GenerateMatrixes(2);
        auto sum_1 = SumMatrixes(mtxs[0], mtxs[1]);
        auto sum_2 = SumMatrixes(MakeTranspose(mtxs[0]), MakeTranspose(mtxs[1]));
        EXPECT_EQ(sum_1.data, sum_2.data);
        EXPECT_EQ(sum_1.height, sum_2.height);
        EXPECT_EQ(sum_1.width, sum_2.width);
    }
}

TEST(Perfomance, TransposeMatrix) {
    for (size_t i = 0; i < 1000; i++) {
        int num_of_matrix = gen() % 6;
        auto mtxs = GenerateMatrixes(num_of_matrix);
        std::vector<std::future<Matrix>> futures(num_of_matrix);
        
        auto worker = get_new_worker();
        for (size_t i = 0; i < num_of_matrix; i++) {
            futures[i] = worker->AsyncProcess(mtxs[i]);
        }

        for (size_t i = 0; i < num_of_matrix; i++) {
            auto cur_mtx = futures[i].get();
            auto transposed_mtx = MakeTranspose(mtxs[i]);
            ASSERT_EQ(cur_mtx.data, transposed_mtx.data);
            ASSERT_EQ(cur_mtx.height, transposed_mtx.height);
            ASSERT_EQ(cur_mtx.width, transposed_mtx.width);
        }
    }
}
