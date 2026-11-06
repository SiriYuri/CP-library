#pragma once

#include "CP/geometry/point.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

template <class T>
struct Line {
    Point<T> a, b;

    Line(Point<T> _a = Point<T>(), Point<T> _b = Point<T>()) : a(_a), b(_b) { }

    template <class S>
    operator Line<S>() const { return Line<S>(Point<S>(a), Point<S>(b)); }

    friend std::istream &operator>>(std::istream &is, Line &l) { return is >> l.a.x >> l.a.y >> l.b.x >> l.b.y; }

    friend std::ostream &operator<<(std::ostream &os, const Line &l) { return os << '<' << l.a << ", " << l.b << '>'; }
};

using Li = Line<i64>;
using Ld = Line<f64>;

template <class T>
bool onLine(Point<T> a, Point<T> b, Point<T> c) { return sign(cross(b, a, c)) == 0; } // a 是否在直线 bc 上

template <class T>
bool onLine(Point<T> p, Line<T> l) { return onLine(l.a, p, l.b); } // p 是否在直线 l 上

template <class T>
bool left(Point<T> p, Line<T> l) { return sign(cross(l.b, p, l.a)) == 1; } // p 是否严格在直线 ab 的左侧

template <class T>
bool sameSide(Point<T> p1, Point<T> p2, Line<T> l) { // p1、p2 是否在 l 同侧
    int s1 = sign(cross(p1, l.a, l.b)), s2 = sign(cross(p2, l.a, l.b));
    return s1 == s2 && s1 != 0;
}

template <class T>
bool differentSide(Point<T> p1, Point<T> p2, Line<T> l) { // p1、p2 是否在 l 异侧
    int s1 = sign(cross(p1, l.a, l.b)), s2 = sign(cross(p2, l.a, l.b));
    return s1 == -s2 && s1 != 0;
}

template <class T>
bool parallel(Line<T> l1, Line<T> l2) { return sign(cross(l1.a - l1.b, l2.a - l2.b)) == 0; } // 是否平行

template <class T>
bool vertical(Line<T> l1, Line<T> l2) { return sign(dot(l1.a - l1.b, l2.a - l2.b)) == 0; } // 是否垂直

inline Pd lineIntersection(Ld l1, Ld l2) { // 直线交点
    f64 val = cross(l2.b - l2.a, l1.a - l2.a) / cross(l2.b - l2.a, l1.a - l1.b);
    return l1.a + (l1.b - l1.a) * val;
}

inline std::pair<Pd, f64> project(Pd p, Ld l) { // [点到直线垂点, 点到直线距离]
    Pd ab = l.b - l.a;
    f64 t = dot(p - l.a, ab) / dot(ab, ab);
    Pd foot = l.a + ab * t;
    return {foot, distance(p, foot)};
}

template <class T>
bool onSegment(Point<T> p, Line<T> l, bool incl = true) { // 点是否在线段上 在端点上默认也算
    if (sign(cross(p, l.a, l.b)) != 0) { return false; }
    int rel = sign(dot(p - l.a, p - l.b));
    return incl ? rel <= 0 : rel < 0;
}

inline std::pair<Pd, f64> pointToSegment(Pd p, Ld l) { // 点到线段最近点和距离
    if (distance(l.a, l.b) < EPS) { return {l.a, distance(p, l.a)}; }
    if (sign(dot(p, l.b, l.a)) == -1) { return {l.a, distance(p, l.a)}; }
    if (sign(dot(p, l.a, l.b)) == -1) { return {l.b, distance(p, l.b)}; }
    return project(p, l);
}

inline Ld median(Ld l) { // 线段中垂线
    Pd mid = (l.a + l.b) / 2.0l;
    f64 dx = l.b.x - l.a.x, dy = l.b.y - l.a.y;
    Pd dir = {-dy, dx};
    return {mid, mid + dir};
}

inline std::tuple<int, Pd, Pd> segmentIntersection(Ld l1, Ld l2) { // 0: 无交点 1: 严格内部交点 2: 重叠线段 3: 交于端点或重叠成一点
    Pd &a = l1.a, &b = l1.b, &c = l2.a, &d = l2.b;
    bool p1 = distance(a, b) < EPS, p2 = distance(c, d) < EPS;
    if (p1 && p2) { return distance(a, c) < EPS ? std::tuple{3, a, a} : std::tuple{0, Pd{}, Pd{}}; }
    if (p1) { return onSegment(a, l2) ? std::tuple{3, a, a} : std::tuple{0, Pd{}, Pd{}}; }
    if (p2) { return onSegment(c, l1) ? std::tuple{3, c, c} : std::tuple{0, Pd{}, Pd{}}; }

    auto [min1x, max1x] = std::minmax(a.x, b.x);
    auto [min1y, max1y] = std::minmax(a.y, b.y);
    auto [min2x, max2x] = std::minmax(c.x, d.x);
    auto [min2y, max2y] = std::minmax(c.y, d.y);
    if (max1x < min2x - EPS || max2x < min1x - EPS || max1y < min2y - EPS || max2y < min1y - EPS) { return {0, { }, { }}; }

    Pd v1 = b - a, v2 = d - c;
    f64 val = cross(v1, v2);
    if (sign(val) == 0) {
        if (sign(cross(c, b, a)) != 0) { return {0, { }, { }}; }

        auto proj = [&] (Pd p) -> f64 { return dot(p - a, v1); };

        f64 t1 = proj(a), t2 = proj(b), t3 = proj(c), t4 = proj(d);
        if (t1 > t2) { std::swap(t1, t2); }
        if (t3 > t4) { std::swap(t3, t4); }
        f64 lo = std::max(t1, t3), hi = std::min(t2, t4);
        if (lo > hi + EPS) { return {0, { }, { }}; }
        Pd pLo = a + v1 * (lo / dot(v1, v1));
        Pd pHi = a + v1 * (hi / dot(v1, v1));
        if (distance(pLo, pHi) < EPS) { return {3, pLo, pHi}; }
        return {2, pLo, pHi};
    }

    Pd p = lineIntersection(l1, l2);
    if (onSegment(p, l1) && onSegment(p, l2)) {
        bool onend = !onSegment(p, l1, false) || !onSegment(p, l2, false);
        return {onend ? 3 : 1, p, p};
    }
    return {0, { }, { }};
}

template <class T>
bool segmentIntersect(Line<T> l1, Line<T> l2) { // 线段是否相交 含端点
    Point<T> s1 = l1.a, e1 = l1.b, s2 = l2.a, e2 = l2.b;
    auto [min1x, max1x] = std::minmax(s1.x, e1.x);
    auto [min1y, max1y] = std::minmax(s1.y, e1.y);
    auto [min2x, max2x] = std::minmax(s2.x, e2.x);
    auto [min2y, max2y] = std::minmax(s2.y, e2.y);
    if (max1x < min2x - EPS || max2x < min1x - EPS || max1y < min2y - EPS || max2y < min1y - EPS) { return false; }
    return sign(cross(s1, e1, s2)) * sign(cross(s1, e1, e2)) <= 0 && sign(cross(s2, e2, s1)) * sign(cross(s2, e2, e1)) <= 0;
}

inline f64 slope(Ld l) { return (l.a.y - l.b.y) / (l.a.x - l.b.x); } // 斜率

template <class T>
Frac<T> fracSlope(Point<T> a, Point<T> b) { // 整数点确定直线的精确斜率
    static_assert(std::is_integral_v<T>);
    return {a.y - b.y, a.x - b.x};
}

template <class T>
std::array<T, 3> generalLine(Line<T> l) { // 两点式转 Ax+By=C
    static_assert(std::is_integral_v<T> && std::is_signed_v<T>);
    T A = l.a.y - l.b.y, B = l.b.x - l.a.x, C = A * l.a.x + B * l.a.y;
    if (A == 0 && B == 0) { return {0, 0, 0}; }
    T g = std::gcd(std::gcd(A >= 0 ? A : -A, B >= 0 ? B : -B), C >= 0 ? C : -C);
    A /= g;
    B /= g;
    C /= g;
    if (A < 0 || (A == 0 && B < 0)) { A = -A, B = -B, C = -C; }
    return {A, B, C};
}

inline Ld generalLine(f64 A, f64 B, f64 C) { // Ax+By=C 转两点式
    if (std::abs(B) > EPS) { return {{0, C / B}, {1, (C - A) / B}}; }
    return {{C / A, 0}, {C / A, 1}};
}
