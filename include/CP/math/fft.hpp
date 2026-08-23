#pragma once

#include "CP/types.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

struct Cp {
    f64 x, y;

    Cp(f64 _x = 0, f64 _y = 0) : x(_x), y(_y) {}

    Cp operator+(const Cp &b) const { return {x + b.x, y + b.y}; }
    Cp operator-(const Cp &b) const { return {x - b.x, y - b.y}; }
    Cp operator*(const Cp &b) const { return {x * b.x - y * b.y, x * b.y + y * b.x}; }
};

inline void FFT(std::vector<Cp> &a, int flag) {
    if (a.size() <= 1) return;
    assert((a.size() & (a.size() - 1)) == 0);
    int limit = 0;
    while ((1ULL << limit) < a.size()) limit++;
    std::vector<int> rev(a.size());
    for (int i = 0; i < int(a.size()); i++) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (limit - 1));
        if (i < rev[i]) std::swap(a[i], a[rev[i]]);
    }
    const f64 pi = std::acos(-1.0L);
    for (int mid = 1; mid < int(a.size()); mid <<= 1) {
        Cp root(std::cos(pi / mid), flag * std::sin(pi / mid));
        for (int length = mid << 1, left = 0; left < int(a.size()); left += length) {
            Cp w(1, 0);
            for (int k = 0; k < mid; k++, w = w * root) {
                Cp x = a[left + k], y = w * a[left + mid + k];
                a[left + k] = x + y;
                a[left + mid + k] = x - y;
            }
        }
    }
}

inline std::vector<i64> mul(std::vector<Cp> &a, std::vector<Cp> &b) {
    if (a.empty() || b.empty()) return {};
    int need = int(a.size() + b.size()) - 1;
    int limit = 1;
    while (limit < need) limit <<= 1;
    a.resize(limit);
    b.resize(limit);
    FFT(a, 1);
    FFT(b, 1);
    for (int i = 0; i < limit; i++) a[i] = a[i] * b[i];
    FFT(a, -1);
    std::vector<i64> result(need);
    for (int i = 0; i < need; i++) result[i] = std::llround(a[i].x / limit);
    return result;
}
