#pragma once

#include "CP/types.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <ostream>
#include <tuple>
#include <type_traits>

inline const f64 PI = std::acos(-1.0l);

template <class T, class S>
bool equal(T x, S y) { return -EPS < x - y && x - y < EPS; }

template <class T>
int sign(T x) { return -EPS < x && x < EPS ? 0 : x < 0 ? -1 : 1; }

template <class T>
struct Frac {
    static_assert(std::is_integral_v<T> && std::is_signed_v<T>);

    T x, y;

    Frac(T _x = 0, T _y = 1) : x(_x), y(_y) { normalize(); }

    void normalize() {
        if (y < 0) { x = -x, y = -y; }
        T g = std::gcd(x >= 0 ? x : -x, y);
        x /= g;
        y /= g;
    }

    explicit operator f64() const { return f64(x) / f64(y); }

    Frac &operator+=(const Frac &rhs) & { x = x * rhs.y + rhs.x * y; y *= rhs.y; normalize(); return *this; }

    Frac &operator-=(const Frac &rhs) & { x = x * rhs.y - rhs.x * y; y *= rhs.y; normalize(); return *this; }

    Frac &operator*=(const Frac &rhs) & { x *= rhs.x; y *= rhs.y; normalize(); return *this; }

    Frac &operator/=(const Frac &rhs) & { x *= rhs.y; y *= rhs.x; normalize(); return *this; }

    friend Frac operator+(Frac lhs, const Frac &rhs) { return lhs += rhs; }

    friend Frac operator-(Frac lhs, const Frac &rhs) { return lhs -= rhs; }

    friend Frac operator*(Frac lhs, const Frac &rhs) { return lhs *= rhs; }

    friend Frac operator/(Frac lhs, const Frac &rhs) { return lhs /= rhs; }

    friend Frac operator-(Frac val) { return {-val.x, val.y}; }

    friend bool operator<(const Frac &lhs, const Frac &rhs) { return lhs.x * rhs.y < rhs.x * lhs.y; }

    friend bool operator>(const Frac &lhs, const Frac &rhs) { return rhs < lhs; }

    friend bool operator==(const Frac &lhs, const Frac &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

    friend bool operator!=(const Frac &lhs, const Frac &rhs) { return !(lhs == rhs); }

    friend std::ostream &operator<<(std::ostream &os, const Frac &val) {
        if (val.y == 1) { return os << val.x; }
        return os << val.x << '/' << val.y;
    }
};

using Fi = Frac<i64>;

inline std::tuple<int, f64, f64> quadraticRoots(f64 a, f64 b, f64 c) { // 一元二次方程实根
    if (std::abs(a) < EPS) {
        if (std::abs(b) < EPS) { return {0, 0, 0}; }
        f64 root = -c / b;
        return {1, root, root};
    }
    f64 dt = b * b - 4 * a * c;
    if (dt < -EPS) { return {0, 0, 0}; }
    if (dt < 0) { dt = 0; }
    f64 sq = std::sqrt(dt);
    f64 x1 = (-b - sq) / (2 * a), x2 = (-b + sq) / (2 * a);
    if (x1 > x2) { std::swap(x1, x2); }
    return sq < EPS ? std::tuple{1, x1, x1} : std::tuple{2, x1, x2};
}
