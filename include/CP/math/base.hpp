#pragma once

#include "CP/types.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <tuple>
#include <utility>
#include <vector>

inline i64 power(i64 x, i64 exp, i64 mod) {
    x %= mod;
    if (x < 0) x += mod;
    i64 ans = 1 % mod;
    for (; exp > 0; exp >>= 1, x = i64(i128(x) * x % mod)) {
        if (exp & 1) ans = i64(i128(ans) * x % mod);
    }
    return ans;
}

inline i64 Floor(i64 a, i64 b) {
    i64 quotient = a / b, remainder = a % b;
    return quotient - (remainder < 0);
}

inline i64 Ceil(i64 a, i64 b) {
    i64 quotient = a / b, remainder = a % b;
    return quotient + (remainder > 0);
}

inline i64 exgcd(i64 a, i64 b, i64 &x, i64 &y) {
    if (!b) return x = 1, y = 0, a;
    i64 g = exgcd(b, a % b, x, y);
    std::tie(x, y) = std::make_tuple(y, x - a / b * y);
    return g;
}

inline auto exgcdFunction(i64 a, i64 b, i64 w) {
    assert(w % std::gcd(a, b) == 0);
    i64 x, y, g = exgcd(a, b, x, y);
    i64 x0 = x * (w / g), y0 = y * (w / g);
    i64 dx = b / g, dy = a / g;
    i64 smin = Ceil(1 - x0, dx), smax = Floor(y0 - 1, dy);
    if (smin > smax) return std::make_tuple(1LL, -1LL, x0 + smin * dx, -1LL, y0 - smax * dy, -1LL, -1LL, -1LL);
    i64 count = smax - smin + 1;
    i64 xmin = x0 + smin * dx, xmax = x0 + smax * dx;
    i64 ymin = y0 - smax * dy, ymax = y0 - smin * dy;
    return std::make_tuple(2LL, count, xmin, xmax, ymin, ymax, dx, dy);
}

inline i64 excrt(const std::vector<std::pair<i64, i64>> &equations) {
    i64 ans = 0, modulus = 1;
    int n = int(equations.size()) - 1;
    for (int i = 1; i <= n; i++) {
        auto [residue, mod] = equations[i];
        i64 x, y;
        i64 difference = (residue - ans % mod + mod) % mod;
        i64 g = exgcd(modulus, mod, x, y);
        if (difference % g) return -1;
        i64 step = mod / g;
        x = i64(i128(x) * (difference / g) % step);
        if (x < 0) x += step;
        i128 nextModulus = i128(modulus) * step;
        ans = i64((i128(ans) + i128(x) * modulus) % nextModulus);
        modulus = i64(nextModulus);
        if (ans < 0) ans += modulus;
    }
    return ans;
}

template <class T>
auto gauss(std::vector<std::vector<T>> function) {
    int n = int(function.size()) - 1, m = int(function[1].size()) - 1, variables = m - 1;
    int row = 1, col = 1;
    std::vector<int> pivotRow(variables + 1);
    while (row <= n && col <= variables) {
        int selected = row;
        for (int i = row + 1; i <= n; i++) {
            if (std::abs(function[i][col]) > std::abs(function[selected][col])) selected = i;
        }
        if (std::abs(function[selected][col]) < EPS) {
            col++;
            continue;
        }
        std::swap(function[row], function[selected]);
        pivotRow[col] = row;
        T divisor = function[row][col];
        for (int j = col; j <= m; j++) function[row][j] /= divisor;
        for (int i = 1; i <= n; i++) {
            if (i != row && std::abs(function[i][col]) > EPS) {
                T factor = function[i][col];
                for (int j = col; j <= m; j++) function[i][j] -= factor * function[row][j];
            }
        }
        row++;
        col++;
    }
    for (int i = 1; i <= n; i++) {
        bool allZero = true;
        for (int j = 1; j <= variables; j++) {
            if (std::abs(function[i][j]) > EPS) {
                allZero = false;
                break;
            }
        }
        if (allZero && std::abs(function[i][m]) > EPS) return std::make_tuple(0, std::vector<T>{});
    }
    int rank = row - 1;
    std::vector<T> ans(variables);
    for (int j = 1; j <= variables; j++) {
        if (pivotRow[j]) ans[j - 1] = function[pivotRow[j]][m];
    }
    if (rank < variables) return std::make_tuple(2, ans);
    return std::make_tuple(1, ans);
}
