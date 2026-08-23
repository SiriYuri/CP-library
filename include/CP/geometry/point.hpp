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
    Point &operator+=(T value) & { return x += value, y += value, *this; }
    Point &operator-=(Point p) & { return x -= p.x, y -= p.y, *this; }
    Point &operator-=(T value) & { return x -= value, y -= value, *this; }
    Point &operator*=(T value) & { return x *= value, y *= value, *this; }
    Point &operator/=(T value) & { return x /= value, y /= value, *this; }

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
T Dot(Point<T> a, Point<T> b) { return a.x * b.x + a.y * b.y; } // 点乘

template <class T>
T Dot(Point<T> p1, Point<T> p2, Point<T> p0) { return Dot(p1 - p0, p2 - p0); } // (p1 - p0) * (p2 - p0)

template <class T>
T Cross(Point<T> a, Point<T> b) { return a.x * b.y - a.y * b.x; } // 叉乘

template <class T>
T Cross(Point<T> p1, Point<T> p2, Point<T> p0) { return Cross(p1 - p0, p2 - p0); } // (p1 - p0) ^ (p2 - p0)

template <class T>
f64 Distance(T x1, T y1, T x2, T y2) { // 欧几里得距离
    f64 dx = f64(x1) - f64(x2), dy = f64(y1) - f64(y2);
    return std::sqrt(dx * dx + dy * dy);
}

template <class T>
f64 Distance(Point<T> a, Point<T> b) { return Distance(a.x, a.y, b.x, b.y); }

template <class T>
T Ex_Distance(T x1, T y1, T x2, T y2) { return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2); } // 不开根的距离

template <class T>
T Ex_Distance(Point<T> a, Point<T> b) { return Ex_Distance(a.x, a.y, b.x, b.y); }

template <class T>
T Manhattan_Distance(Point<T> a, Point<T> b) { return std::abs(a.x - b.x) + std::abs(a.y - b.y); } // 曼哈顿距离

inline Pd Standardize(Pd vec) { return vec / Distance(vec, Pd{}); }

inline f64 ToDeg(f64 angle) { return angle * 180.0l / PI; } // 弧度转角度

inline f64 ToArc(f64 angle) { return angle * PI / 180.0l; } // 角度转弧度

inline f64 CosinesRule(f64 a, f64 b, f64 c) { // 余弦定理求边 c 对应角的弧度 注意精度问题
    f64 value = (a * a + b * b - c * c) / (2.0l * a * b);
    return std::acos(std::clamp(value, -1.0l, 1.0l));
}

inline f64 VectorAngle(Pd a, Pd b, bool absolute = true) { // 求两向量夹角的弧度 默认返回无符号角度 注意精度问题
    f64 angle = std::atan2(Cross(a, b), Dot(a, b));
    return absolute ? std::abs(angle) : angle;
}

inline Pd Rotate(Pd p, Pd center, f64 rad, bool clockwise = false) { // p 绕 center 旋转 rad 弧度 默认逆时针
    auto [dx, dy] = p - center;
    f64 angle = clockwise ? -rad : rad, c = std::cos(angle), s = std::sin(angle);
    return {dx * c - dy * s + center.x, dx * s + dy * c + center.y};
}

template <class T>
Point<T> Rotate90(Point<T> p, Point<T> center = { }, bool clockwise = false) { // 绕 center 旋转 90 度
    Point<T> vec = p - center;
    Point<T> rotated = clockwise ? Point<T>{vec.y, -vec.x} : Point<T>{-vec.y, vec.x};
    return center + rotated;
}
