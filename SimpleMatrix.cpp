//
// Created by root on 19-1-29.
//

#include "SimpleMatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <functional>

SimpleMatrix::SimpleMatrix(int rownum, int colnum)
        : _matrix(rownum, std::vector<double>(colnum, 0)) {}

void SimpleMatrix::fill_random() {
    srand((int) time(0));
    for (int i = 0; i < this->get_rownum(); i++) {
        for (int j = 0; j < this->get_colnum(); j++) {
            this->_matrix[i][j] = rand() % 10;
        }
    }
}

std::shared_ptr<SimpleMatrix> SimpleMatrix::get_inverse() {
    std::shared_ptr<SimpleMatrix> adjugate = this->get_adjugate();
    double det = this->get_determinant();
    for (int i = 0; i < this->get_rownum(); i++) {
        for (int j = 0; j < this->get_colnum(); j++) {
            adjugate->get_matrix()[i][j] /= det;
        }
    }
    return adjugate;
}

std::shared_ptr<SimpleMatrix> SimpleMatrix::get_transpose() {
    std::shared_ptr<SimpleMatrix> result = std::make_shared<SimpleMatrix>(this->get_colnum(), this->get_rownum());
    for (int i = 0; i < this->get_rownum(); i++) {
        for (int j = 0; j < this->get_colnum(); j++) {
            result->get_matrix()[j][i] = this->_matrix[i][j];
        }
    }
    return result;
}

std::shared_ptr<SimpleMatrix> SimpleMatrix::multiply(SimpleMatrix &matrix) {

    if (matrix.get_rownum() != this->get_colnum()) {
        std::cout << "Error" << std::endl;
        return std::shared_ptr<SimpleMatrix>(nullptr);
    }

    std::shared_ptr<SimpleMatrix> result = std::make_shared<SimpleMatrix>(this->get_rownum(), matrix.get_colnum());


    std::vector<std::thread> threads;

    int rowbatch = this->get_rownum() / threadnum;
    int row_remain = this->get_rownum() % threadnum;

    for (int i = 0; i < threadnum; i++) {
        int row_start = i * rowbatch;
        int row_end = (i + 1) * rowbatch;

        if (i == threadnum - 1) {
            row_end += row_remain;
        }
        std::thread t(std::bind(&SimpleMatrix::_sub_multiply, this, result, row_start, row_end, matrix));
        threads.push_back(move(t));
    }

    for (std::thread &t: threads) {
        t.join();
    }

    return result;

}

void
SimpleMatrix::_sub_multiply(std::shared_ptr<SimpleMatrix> result, int row_start, int row_end, SimpleMatrix &matrix) {
    for (int i = row_start; i < row_end; i++) {
        for (int j = 0; j < matrix.get_colnum(); j++) {
            result->get_matrix()[i][j] = _dot_multiply(i, j, matrix);
        }
    }
}

std::shared_ptr<SimpleMatrix> SimpleMatrix::get_adjugate() {
    std::shared_ptr<SimpleMatrix> result = std::make_shared<SimpleMatrix>(this->get_rownum(), this->get_colnum());
    for (int i = 0; i < this->get_rownum(); i++) {
        for (int j = 0; j < this->get_colnum(); j++) {
            double sign = (i + j) % 2 == 0 ? 1 : -1;
            result->get_matrix()[i][j] = sign * this->_delete(*this, i, j)->get_determinant();
        }
    }
    return result->get_transpose();
}

double SimpleMatrix::get_determinant() {
    return _determinant(*this);
}

double SimpleMatrix::_determinant(SimpleMatrix &matrix) {
    if (matrix.get_rownum() != matrix.get_colnum()) {
        std::cout << "Error" << std::endl;
        return 0;
    }
    if (matrix.get_rownum() == 2) {
        return matrix.get_matrix()[0][0] * matrix.get_matrix()[1][1] -
               matrix.get_matrix()[0][1] * matrix.get_matrix()[1][0];
    }

    double result = 0;

    for (int i = 0; i < matrix.get_colnum(); i++) {
        double sign = i % 2 == 0 ? 1 : -1;
        result += sign * matrix.get_matrix()[0][i] * _determinant(*(this->_delete(matrix, 0, i)));
    }
    return result;
}

void SimpleMatrix::print_matrix() {
    for (std::vector<double> const &row: this->_matrix) {
        for (double const &element: row) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void SimpleMatrix::print_matrix2() {
    for (double const &element:this->_matrix.front()) {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    std::cout << "...." << std::endl;

    for (double const &element:this->_matrix.back()) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

int SimpleMatrix::get_rownum() {
    return this->_matrix.size();
}

int SimpleMatrix::get_colnum() {
    return this->_matrix[0].size();
}

double SimpleMatrix::_dot_multiply(int row_idx, int col_idx, SimpleMatrix &matrix) {
    double result = 0;
    for (int i = 0; i < _matrix[row_idx].size(); i++) {
        result += this->_matrix[row_idx][i] * matrix.get_matrix()[i][col_idx];
    }
    return result;
}

std::vector<std::vector<double>> &SimpleMatrix::get_matrix() {
    return this->_matrix;
}

std::shared_ptr<SimpleMatrix> SimpleMatrix::_delete(SimpleMatrix &matrix, int row_idx, int col_idx) {
    std::shared_ptr<SimpleMatrix> result = std::make_shared<SimpleMatrix>(matrix.get_rownum() - 1,
                                                                          matrix.get_colnum() - 1);
    for (int i = 0; i < row_idx; i++) {
        for (int j = 0; j < col_idx; j++) {
            result->get_matrix()[i][j] = matrix.get_matrix()[i][j];
        }
    }
    for (int i = 0; i < row_idx; i++) {
        for (int j = col_idx + 1; j < matrix.get_colnum(); j++) {
            result->get_matrix()[i][j - 1] = matrix.get_matrix()[i][j];
        }
    }

    for (int i = row_idx + 1; i < matrix.get_rownum(); i++) {
        for (int j = 0; j < col_idx; j++) {
            result->get_matrix()[i - 1][j] = matrix.get_matrix()[i][j];
        }
    }
    for (int i = row_idx + 1; i < matrix.get_rownum(); i++) {
        for (int j = col_idx + 1; j < matrix.get_colnum(); j++) {
            result->get_matrix()[i - 1][j - 1] = matrix.get_matrix()[i][j];
        }
    }

    return result;

}

void SimpleMatrix::clear() {
    this->get_matrix().clear();
}