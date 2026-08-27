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

inline void fft(std::vector<Cp> &a, int flag) {
    if (a.size() <= 1) { return; }
    assert((a.size() & (a.size() - 1)) == 0);
    int lim = 0;
    while ((1ULL << lim) < a.size()) lim++;
    std::vector<int> rev(a.size());
    for (int i = 0; i < int(a.size()); i++) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (lim - 1));
        if (i < rev[i]) { std::swap(a[i], a[rev[i]]); }
    }
    const f64 pi = std::acos(-1.0L);
    for (int mid = 1; mid < int(a.size()); mid <<= 1) {
        Cp root(std::cos(pi / mid), flag * std::sin(pi / mid));
        for (int len = mid << 1, left = 0; left < int(a.size()); left += len) {
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
    if (a.empty() || b.empty()) { return {}; }
    int need = int(a.size() + b.size()) - 1;
    int lim = 1;
    while (lim < need) lim <<= 1;
    a.resize(lim);
    b.resize(lim);
    fft(a, 1);
    fft(b, 1);
    for (int i = 0; i < lim; i++) a[i] = a[i] * b[i];
    fft(a, -1);
    std::vector<i64> res(need);
    for (int i = 0; i < need; i++) res[i] = std::llround(a[i].x / lim);
    return res;
}
