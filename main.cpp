#include <iostream>
#include "SimpleMatrix.h"
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>

std::mutex m;

std::shared_ptr<SimpleMatrix> estimate_ols_params(SimpleMatrix& x, SimpleMatrix& y){
    std::shared_ptr<SimpleMatrix> beta = x.get_transpose()->multiply(x)->get_inverse()->multiply(
            *(x.get_transpose()))->multiply(y);

    beta->print_matrix();
    return beta;
}

void test_regression() {
    SimpleMatrix x(150000, 6);
    x.fill_random();
    for (int i = 0; i < x.get_rownum(); i++) {
        x.get_matrix()[i][5] = 1;
    }
    x.print_matrix2();

    SimpleMatrix y(150000, 1);
    //y.fill_random();
    for (int i = 0; i < x.get_rownum(); i++) {
        double ramdon = ((double) rand() / (RAND_MAX));
        double sign = ramdon < 0.5 ? 1 : -1;
        y.get_matrix()[i][0] = x.get_matrix()[i][0] * 5 + x.get_matrix()[i][1] * 6 + x.get_matrix()[i][2] * 7 +
                               x.get_matrix()[i][3] * 8 + x.get_matrix()[i][4] * 9 + 10 + ramdon * sign;
    }
    y.print_matrix2();

    estimate_ols_params(x,y);
}

void generate_data(SimpleMatrix &xbuffer, SimpleMatrix &ybuffer) {
    while (1) {
        SimpleMatrix x(1, 6);
        x.fill_random();
        x.get_matrix()[0][5] = 1;

        SimpleMatrix y(1, 1);
        double ramdon = ((double) rand() / (RAND_MAX));
        double sign = ramdon < 0.5 ? 1 : -1;
        y.get_matrix()[0][0] = x.get_matrix()[0][0] * 5 + x.get_matrix()[0][1] * 6 + x.get_matrix()[0][2] * 7 +
                               x.get_matrix()[0][3] * 8 + x.get_matrix()[0][4] * 9 + 10 + ramdon * sign;
        m.lock();
        xbuffer.get_matrix().push_back(x.get_matrix()[0]);
        ybuffer.get_matrix().push_back(y.get_matrix()[0]);
        m.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void read_buffer(SimpleMatrix &xbuffer, SimpleMatrix &ybuffer, SimpleMatrix &x, SimpleMatrix &y) {
    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        m.lock();
        x.clear();
        y.clear();
        for (int i = 0; i < xbuffer.get_rownum(); i++) {
            x.get_matrix().push_back(xbuffer.get_matrix()[i]);
            y.get_matrix().push_back(ybuffer.get_matrix()[i]);
        }
        xbuffer.clear();
        ybuffer.clear();
        m.unlock();
        estimate_ols_params(x,y);
    }
}

int main() {
    srand((int) time(0));

    //question 1
    /**
X随机生成
Y 的数据由以下式子生产，带有随机扰动
y=5*x1+6*x2+7*x3+8*x4+9*x5+10+rand()
由生成的数据能回归计算出非常接近beta
如果生成数据时去除扰动项，回归出完全一样的beta

效率上，假设X的维度为k，行数为n，计算时XT 与 x相乘的乘法次数为k*n*k, 求逆之后与XT相乘，乘法次数为k*k*n,代码中的k=5，若将k视为常数，求逆也是常数时间，则程序是O(n)的。
在矩阵乘法中也使用了多线程，加速比较耗时的矩阵乘法计算
     */
    test_regression();



    //question 2
    SimpleMatrix xbuffer(0, 6);
    SimpleMatrix ybuffer(0, 1);
    SimpleMatrix x(0, 6);
    SimpleMatrix y(0, 1);
    std::thread t(generate_data, std::ref(xbuffer), std::ref(ybuffer));
    read_buffer(xbuffer, ybuffer, x, y);
    t.join();


    return 0;
}