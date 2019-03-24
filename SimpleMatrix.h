//
// Created by root on 19-1-29.
//

#ifndef SIMPLEMATRIX_SIMPLEMATRIX_H
#define SIMPLEMATRIX_SIMPLEMATRIX_H

#include <iostream>
#include <vector>
#include <memory>

#define threadnum 2

class SimpleMatrix {

public:
    SimpleMatrix(int rownum, int colnum);

    std::vector<std::vector<double>> &get_matrix();

    void fill_random();

    std::shared_ptr<SimpleMatrix> get_inverse();

    std::shared_ptr<SimpleMatrix> get_transpose();

    //support multi thread
    std::shared_ptr<SimpleMatrix> multiply(SimpleMatrix &matrix);

    std::shared_ptr<SimpleMatrix> get_adjugate();

    double get_determinant();

    void print_matrix();

    void print_matrix2();

    int get_rownum();

    int get_colnum();

    void clear();

private:
    std::vector<std::vector<double>> _matrix;

    double _dot_multiply(int row_idx, int col_idx, SimpleMatrix &matrix);

    void _sub_multiply(std::shared_ptr<SimpleMatrix> result, int row_start, int row_end,
                       SimpleMatrix &matrix);

    double _determinant(SimpleMatrix &matrix);

    std::shared_ptr<SimpleMatrix> _delete(SimpleMatrix &matrix, int row_idx, int col_idx);
};


#endif //SIMPLEMATRIX_SIMPLEMATRIX_H
