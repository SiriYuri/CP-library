#pragma once

#include "CP/types.hpp"

#include <cassert>
#include <optional>
#include <utility>
#include <vector>

template <typename T = i64>
struct Matrix {
    int n;
    std::vector<std::vector<T>> val;

    explicit Matrix(int _n = 0) : n(_n), val(n, std::vector<T>(n)) {}

    std::vector<T> &operator[](int idx) { return val[idx]; }

    const std::vector<T> &operator[](int idx) const { return val[idx]; }

    Matrix &operator+=(const Matrix &rhs) {
        assert(n == rhs.n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) val[i][j] += rhs[i][j];
        }
        return *this;
    }

    Matrix &operator*=(const Matrix &rhs) { return *this = *this * rhs; }

    friend Matrix operator+(Matrix lhs, const Matrix &rhs) { return lhs += rhs; }

    friend Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
        assert(lhs.n == rhs.n);
        Matrix res(lhs.n);
        for (int i = 0; i < res.n; i++) {
            for (int k = 0; k < res.n; k++) {
                for (int j = 0; j < res.n; j++) res[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
        return res;
    }

    std::optional<Matrix> inv() const {
        Matrix lhs = *this;
        Matrix rhs(n);
        for (int i = 0; i < n; i++) rhs[i][i] = T(1);
        for (int col = 0; col < n; col++) {
            int piv = col;
            while (piv < n && lhs[piv][col] == T()) piv++;
            if (piv == n) { return std::nullopt; }
            std::swap(lhs[piv], lhs[col]);
            std::swap(rhs[piv], rhs[col]);
            T ivs = T(1) / lhs[col][col];
            for (int j = 0; j < n; j++) {
                lhs[col][j] *= ivs;
                rhs[col][j] *= ivs;
            }
            for (int i = 0; i < n; i++) {
                if (i == col || lhs[i][col] == T()) { continue; }
                T fac = lhs[i][col];
                for (int j = 0; j < n; j++) {
                    lhs[i][j] -= fac * lhs[col][j];
                    rhs[i][j] -= fac * rhs[col][j];
                }
            }
        }
        return rhs;
    }

    static Matrix power(Matrix base, i64 exp) {
        Matrix res(base.n);
        for (int i = 0; i < base.n; i++) res[i][i] = T(1);
        for (; exp > 0; exp >>= 1, base *= base) {
            if (exp & 1) { res *= base; }
        }
        return res;
    }
};
