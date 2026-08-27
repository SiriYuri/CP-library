#pragma once

#include "CP/geometry/line.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

template <class T>
struct Circle {
    Point<T> O;
    T R;

    Circle(Point<T> _O = Point<T>(), T _R = T()) : O(_O), R(_R) { }

    template <class S>
    operator Circle<S>() const { return Circle<S>(Point<S>(O), S(R)); }

    friend std::istream &operator>>(std::istream &is, Circle &circle) { return is >> circle.O.x >> circle.O.y >> circle.R; }

    friend std::ostream &operator<<(std::ostream &os, const Circle &circle) { return os << '<' << circle.O << ", " << circle.R << '>'; }
};

using Ci = Circle<i64>;
using Cd = Circle<f64>;

inline Pd circumferencePoint(Cd circle, f64 rad) { return circle.O + Pd{std::cos(rad), std::sin(rad)} * circle.R; } // 圆上极角为 rad 的点

inline std::pair<Pd, f64> pointToCircle(Pd p, Cd cir) { // [圆上最近点, 点到圆周的距离]
    Pd vec = p - cir.O;
    f64 dis = distance(p, cir.O);
    if (dis < EPS) {
        Pd near = cir.O + Pd{cir.R, 0};
        return {near, cir.R};
    }
    Pd near = cir.O + vec * (cir.R / dis);
    return {near, std::abs(dis - cir.R)};
}

inline std::tuple<int, Pd, Pd> lineCircleIntersection(Ld l, Cd cir) { // 0: 相离 1: 相切 2: 两个交点
    auto [foot, dis] = project(cir.O, l);
    f64 rem = cir.R * cir.R - dis * dis;
    if (rem < -EPS) { return {0, { }, { }}; }
    if (rem < 0) { rem = 0; }
    Pd off = standardize(l.b - l.a) * std::sqrt(rem);
    if (distance(off, Pd{}) < EPS) { return {1, foot, foot}; }
    return {2, foot - off, foot + off};
}

inline std::tuple<int, Pd, Pd> segmentCircleIntersection(Ld l, Cd cir) { // 线段与圆周的交点
    if (distance(l.a, l.b) < EPS) {
        return std::abs(distance(l.a, cir.O) - cir.R) < EPS ? std::tuple{1, l.a, l.a} : std::tuple{0, Pd{}, Pd{}};
    }
    auto [type, p1, p2] = lineCircleIntersection(l, cir);
    std::vector<Pd> pts;
    if (type >= 1 && onSegment(p1, l)) { pts.emplace_back(p1); }
    if (type == 2 && onSegment(p2, l) && distance(p1, p2) >= EPS) { pts.emplace_back(p2); }
    if (pts.empty()) { return {0, { }, { }}; }
    if (pts.size() == 1) { return {1, pts[0], pts[0]}; }
    return {2, pts[0], pts[1]};
}

inline std::tuple<int, Pd, Pd> circleCircleIntersection(Cd c1, Cd c2) { // -1: 重合 0: 无交点 1: 相切 2: 两个交点
    Pd dir = c2.O - c1.O;
    f64 dis = distance(c1.O, c2.O);
    if (dis < EPS) {
        if (std::abs(c1.R - c2.R) < EPS) { return {-1, { }, { }}; }
        return {0, { }, { }};
    }
    if (dis > c1.R + c2.R + EPS || dis < std::abs(c1.R - c2.R) - EPS) { return {0, { }, { }}; }
    f64 x = (dis * dis + c1.R * c1.R - c2.R * c2.R) / (2 * dis);
    f64 h2 = c1.R * c1.R - x * x;
    if (h2 < 0) { h2 = 0; }
    Pd base = c1.O + dir * (x / dis);
    Pd off = Pd{-dir.y, dir.x} * (std::sqrt(h2) / dis);
    if (distance(off, Pd{}) < EPS) { return {1, base, base}; }
    return {2, base - off, base + off};
}

inline f64 circleIntersectionArea(Cd c1, Cd c2) { // 两圆交面积
    f64 dis = distance(c1.O, c2.O);
    if (dis >= c1.R + c2.R - EPS) { return 0; }
    if (dis <= std::abs(c1.R - c2.R) + EPS) {
        f64 rad = std::min(c1.R, c2.R);
        return PI * rad * rad;
    }
    f64 ang1 = std::acos(std::clamp((dis * dis + c1.R * c1.R - c2.R * c2.R) / (2 * dis * c1.R), f64(-1), f64(1)));
    f64 ang2 = std::acos(std::clamp((dis * dis + c2.R * c2.R - c1.R * c1.R) / (2 * dis * c2.R), f64(-1), f64(1)));
    return c1.R * c1.R * ang1 + c2.R * c2.R * ang2 - dis * c1.R * std::sin(ang1);
}

inline std::pair<bool, Cd> circleFromThreePoints(Pd a, Pd b, Pd c) { // 三点定圆
    if (sign(cross(b - a, c - a)) == 0) { return {false, { }}; }

    Ld l1 = median({a, b}), l2 = median({a, c});
    Pd cen = lineIntersection(l1, l2);
    return {true, {cen, distance(cen, a)}};
}

inline std::vector<Pd> pointCircleTangents(Pd p, Cd circle) { // 点到圆的切点
    Pd vec = p - circle.O;
    f64 dis2 = dot(vec, vec), rad2 = circle.R * circle.R;
    if (dis2 < rad2 - EPS) { return { }; }
    if (std::abs(dis2 - rad2) < EPS) { return {p}; }
    Pd base = circle.O + vec * (rad2 / dis2);
    Pd off = Pd{-vec.y, vec.x} * (circle.R * std::sqrt(dis2 - rad2) / dis2);
    return {base - off, base + off};
}

inline std::pair<int, std::vector<std::pair<Pd, Pd>>> commonTangents(Cd c1, Cd c2) { // 两圆公切线
    Pd dir = c2.O - c1.O;
    f64 dis2 = dot(dir, dir);
    if (dis2 < EPS && std::abs(c1.R - c2.R) < EPS) { return {-1, { }}; }
    if (dis2 < EPS) { return {0, { }}; }
    std::vector<std::pair<Pd, Pd>> ans;
    for (int type : {-1, 1}) {
        f64 rad = c1.R - type * c2.R;
        f64 h2 = dis2 - rad * rad;
        if (h2 < -EPS) { continue; }
        h2 = std::max(f64(0), h2);
        for (int side : {-1, 1}) {
            Pd vec = (dir * rad + Pd{-dir.y, dir.x} * (std::sqrt(h2) * side)) / dis2;
            ans.emplace_back(c1.O + vec * c1.R, c2.O + vec * (type * c2.R));
            if (h2 < EPS) { break; }
        }
    }
    return {int(ans.size()), ans};
}
