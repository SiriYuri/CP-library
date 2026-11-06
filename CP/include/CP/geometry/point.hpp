#pragma once

#include "CP/geometry/base.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

template <class T>
struct Point {
    T x, y;

    Point(T _x = 0, T _y = 0) : x(_x), y(_y) { }

    template <class S>
    operator Point<S>() const { return Point<S>(S(x), S(y)); }

    Point &operator+=(Point p) & { return x += p.x, y += p.y, *this; }

    Point &operator+=(T val) & { return x += val, y += val, *this; }

    Point &operator-=(Point p) & { return x -= p.x, y -= p.y, *this; }

    Point &operator-=(T val) & { return x -= val, y -= val, *this; }

    Point &operator*=(T val) & { return x *= val, y *= val, *this; }

    Point &operator/=(T val) & { return x /= val, y /= val, *this; }

    Point operator-() const { return Point(-x, -y); }

    friend Point operator+(Point lhs, Point rhs) { return lhs += rhs; }

    friend Point operator+(Point lhs, T rhs) { return lhs += rhs; }

    friend Point operator-(Point lhs, Point rhs) { return lhs -= rhs; }

    friend Point operator-(Point lhs, T rhs) { return lhs -= rhs; }

    friend Point operator*(Point lhs, T rhs) { return lhs *= rhs; }

    friend Point operator*(T lhs, Point rhs) { return rhs *= lhs; }

    friend Point operator/(Point lhs, T rhs) { return lhs /= rhs; }

    friend bool operator<(Point lhs, Point rhs) { return lhs.x != rhs.x ? lhs.x < rhs.x : lhs.y < rhs.y; }

    friend bool operator>(Point lhs, Point rhs) { return rhs < lhs; }

    friend bool operator==(Point lhs, Point rhs) { return !(lhs < rhs) && !(rhs < lhs); }

    friend bool operator!=(Point lhs, Point rhs) { return lhs < rhs || rhs < lhs; }

    friend std::istream &operator>>(std::istream &is, Point &p) { return is >> p.x >> p.y; }

    friend std::ostream &operator<<(std::ostream &os, const Point &p) { return os << '(' << p.x << ", " << p.y << ')'; }
};

using Pi = Point<i64>;
using Pd = Point<f64>;

template <class T>
T dot(Point<T> a, Point<T> b) { return a.x * b.x + a.y * b.y; } // 点乘

template <class T>
T dot(Point<T> p1, Point<T> p2, Point<T> p0) { return dot(p1 - p0, p2 - p0); } // (p1 - p0) * (p2 - p0)

template <class T>
T cross(Point<T> a, Point<T> b) { return a.x * b.y - a.y * b.x; } // 叉乘

template <class T>
T cross(Point<T> p1, Point<T> p2, Point<T> p0) { return cross(p1 - p0, p2 - p0); } // (p1 - p0) ^ (p2 - p0)

template <class T>
f64 distance(T x1, T y1, T x2, T y2) { // 欧几里得距离
    f64 dx = f64(x1) - f64(x2), dy = f64(y1) - f64(y2);
    return std::sqrt(dx * dx + dy * dy);
}

template <class T>
f64 distance(Point<T> a, Point<T> b) { return distance(a.x, a.y, b.x, b.y); }

template <class T>
T squaredDistance(T x1, T y1, T x2, T y2) { return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2); } // 不开根的距离

template <class T>
T squaredDistance(Point<T> a, Point<T> b) { return squaredDistance(a.x, a.y, b.x, b.y); }

template <class T>
T manhattanDistance(Point<T> a, Point<T> b) { return std::abs(a.x - b.x) + std::abs(a.y - b.y); } // 曼哈顿距离

inline Pd standardize(Pd vec) { return vec / distance(vec, Pd{}); }

inline f64 toDeg(f64 ang) { return ang * 180.0l / PI; } // 弧度转角度

inline f64 toArc(f64 ang) { return ang * PI / 180.0l; } // 角度转弧度

inline f64 cosinesRule(f64 a, f64 b, f64 c) { // 余弦定理求边 c 对应角的弧度 注意精度问题
    f64 val = (a * a + b * b - c * c) / (2.0l * a * b);
    return std::acos(std::clamp(val, -1.0l, 1.0l));
}

inline f64 vectorAngle(Pd a, Pd b, bool abs = true) { // 求两向量夹角的弧度 默认返回无符号角度 注意精度问题
    f64 ang = std::atan2(cross(a, b), dot(a, b));
    return abs ? std::abs(ang) : ang;
}

inline Pd rotate(Pd p, Pd cen, f64 rad, bool cw = false) { // p 绕 cen 旋转 rad 弧度 默认逆时针
    auto [dx, dy] = p - cen;
    f64 ang = cw ? -rad : rad, c = std::cos(ang), s = std::sin(ang);
    return {dx * c - dy * s + cen.x, dx * s + dy * c + cen.y};
}

template <class T>
Point<T> rotate90(Point<T> p, Point<T> cen = { }, bool cw = false) { // 绕 cen 旋转 90 度
    Point<T> vec = p - cen;
    Point<T> rot = cw ? Point<T>{vec.y, -vec.x} : Point<T>{-vec.y, vec.x};
    return cen + rot;
}
