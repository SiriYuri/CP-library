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

inline Pd CircumferencePoint(Cd circle, f64 rad) { return circle.O + Pd{std::cos(rad), std::sin(rad)} * circle.R; } // 圆上极角为 rad 的点

inline std::pair<Pd, f64> PointToCircle(Pd p, Cd circle) { // [圆上最近点, 点到圆周的距离]
    Pd vec = p - circle.O;
    f64 distance = Distance(p, circle.O);
    if (distance < EPS) {
        Pd nearest = circle.O + Pd{circle.R, 0};
        return {nearest, circle.R};
    }
    Pd nearest = circle.O + vec * (circle.R / distance);
    return {nearest, std::abs(distance - circle.R)};
}

inline std::tuple<int, Pd, Pd> LineCircleIntersection(Ld l, Cd circle) { // 0: 相离 1: 相切 2: 两个交点
    auto [foot, distance] = Project(circle.O, l);
    f64 remain = circle.R * circle.R - distance * distance;
    if (remain < -EPS) return {0, { }, { }};
    if (remain < 0) remain = 0;
    Pd offset = Standardize(l.b - l.a) * std::sqrt(remain);
    if (Distance(offset, Pd{}) < EPS) return {1, foot, foot};
    return {2, foot - offset, foot + offset};
}

inline std::tuple<int, Pd, Pd> SegmentCircleIntersection(Ld l, Cd circle) { // 线段与圆周的交点
    if (Distance(l.a, l.b) < EPS) {
        return std::abs(Distance(l.a, circle.O) - circle.R) < EPS ? std::tuple{1, l.a, l.a} : std::tuple{0, Pd{}, Pd{}};
    }
    auto [type, p1, p2] = LineCircleIntersection(l, circle);
    std::vector<Pd> points;
    if (type >= 1 && Onsegment(p1, l)) points.emplace_back(p1);
    if (type == 2 && Onsegment(p2, l) && Distance(p1, p2) >= EPS) points.emplace_back(p2);
    if (points.empty()) return {0, { }, { }};
    if (points.size() == 1) return {1, points[0], points[0]};
    return {2, points[0], points[1]};
}

inline std::tuple<int, Pd, Pd> CircleCircleIntersection(Cd c1, Cd c2) { // -1: 重合 0: 无交点 1: 相切 2: 两个交点
    Pd direction = c2.O - c1.O;
    f64 distance = Distance(c1.O, c2.O);
    if (distance < EPS) {
        if (std::abs(c1.R - c2.R) < EPS) return {-1, { }, { }};
        return {0, { }, { }};
    }
    if (distance > c1.R + c2.R + EPS || distance < std::abs(c1.R - c2.R) - EPS) return {0, { }, { }};
    f64 x = (distance * distance + c1.R * c1.R - c2.R * c2.R) / (2 * distance);
    f64 h2 = c1.R * c1.R - x * x;
    if (h2 < 0) h2 = 0;
    Pd base = c1.O + direction * (x / distance);
    Pd offset = Pd{-direction.y, direction.x} * (std::sqrt(h2) / distance);
    if (Distance(offset, Pd{}) < EPS) return {1, base, base};
    return {2, base - offset, base + offset};
}

inline f64 CircleIntersectionArea(Cd c1, Cd c2) { // 两圆交面积
    f64 distance = Distance(c1.O, c2.O);
    if (distance >= c1.R + c2.R - EPS) return 0;
    if (distance <= std::abs(c1.R - c2.R) + EPS) {
        f64 radius = std::min(c1.R, c2.R);
        return PI * radius * radius;
    }
    f64 angle1 = std::acos(std::clamp((distance * distance + c1.R * c1.R - c2.R * c2.R) / (2 * distance * c1.R), f64(-1), f64(1)));
    f64 angle2 = std::acos(std::clamp((distance * distance + c2.R * c2.R - c1.R * c1.R) / (2 * distance * c2.R), f64(-1), f64(1)));
    return c1.R * c1.R * angle1 + c2.R * c2.R * angle2 - distance * c1.R * std::sin(angle1);
}

inline std::pair<bool, Cd> CircleFromThreePoints(Pd a, Pd b, Pd c) { // 三点定圆
    if (Sign(Cross(b - a, c - a)) == 0) return {false, { }};
    Ld l1 = Median({a, b}), l2 = Median({a, c});
    Pd center = LineIntersection(l1, l2);
    return {true, {center, Distance(center, a)}};
}

inline std::vector<Pd> PointCircleTangents(Pd p, Cd circle) { // 点到圆的切点
    Pd vec = p - circle.O;
    f64 distance2 = Dot(vec, vec), radius2 = circle.R * circle.R;
    if (distance2 < radius2 - EPS) return { };
    if (std::abs(distance2 - radius2) < EPS) return {p};
    Pd base = circle.O + vec * (radius2 / distance2);
    Pd offset = Pd{-vec.y, vec.x} * (circle.R * std::sqrt(distance2 - radius2) / distance2);
    return {base - offset, base + offset};
}

inline std::pair<int, std::vector<std::pair<Pd, Pd>>> CommonTangents(Cd c1, Cd c2) { // 两圆公切线
    Pd direction = c2.O - c1.O;
    f64 distance2 = Dot(direction, direction);
    if (distance2 < EPS && std::abs(c1.R - c2.R) < EPS) return {-1, { }};
    if (distance2 < EPS) return {0, { }};
    std::vector<std::pair<Pd, Pd>> ans;
    for (int type : {-1, 1}) {
        f64 radius = c1.R - type * c2.R;
        f64 h2 = distance2 - radius * radius;
        if (h2 < -EPS) continue;
        h2 = std::max(f64(0), h2);
        for (int side : {-1, 1}) {
            Pd vec = (direction * radius + Pd{-direction.y, direction.x} * (std::sqrt(h2) * side)) / distance2;
            ans.emplace_back(c1.O + vec * c1.R, c2.O + vec * (type * c2.R));
            if (h2 < EPS) break;
        }
    }
    return {int(ans.size()), ans};
}
