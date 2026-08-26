#pragma once

#include "CP/types.hpp"

#include <cassert>
#include <vector>

template <typename T = i64>
struct Matrix {
    int n;
    std::vector<std::vector<T>> value;

    explicit Matrix(int _n = 0) : n(_n), value(n, std::vector<T>(n)) {}

    std::vector<T> &operator[](int index) { return value[index]; }

    const std::vector<T> &operator[](int index) const { return value[index]; }

    Matrix &operator+=(const Matrix &rhs) {
        assert(n == rhs.n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) value[i][j] += rhs[i][j];
        }
        return *this;
    }

    Matrix &operator*=(const Matrix &rhs) { return *this = *this * rhs; }

    friend Matrix operator+(Matrix lhs, const Matrix &rhs) { return lhs += rhs; }

    friend Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
        assert(lhs.n == rhs.n);
        Matrix result(lhs.n);
        for (int i = 0; i < result.n; i++) {
            for (int k = 0; k < result.n; k++) {
                for (int j = 0; j < result.n; j++) result[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
        return result;
    }

    static Matrix Pow(Matrix base, i64 exponent) {
        Matrix result(base.n);
        for (int i = 0; i < base.n; i++) result[i][i] = T(1);
        for (; exponent > 0; exponent >>= 1, base *= base) {
            if (exponent & 1) result *= base;
        }
        return result;
    }
};
