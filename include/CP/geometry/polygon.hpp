#pragma once

#include "CP/geometry/circle.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <utility>
#include <vector>

template <class T>
std::vector<Point<T>> Convex(std::vector<Point<T>> points) { // 严格凸包 去重并删除边上的共线中间点 逆时针返回且不重复首点
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    int n = int(points.size());
    if (n <= 2) return points;
    std::vector<Point<T>> ans(n * 2);
    int now = -1;
    for (int i = 0; i < n; ++i) {
        while (now > 0 && Sign(Cross(ans[now], points[i], ans[now - 1])) <= 0) --now;
        ans[++now] = points[i];
    }
    int pre = now;
    for (int i = n - 2; i >= 0; --i) {
        while (now > pre && Sign(Cross(ans[now], points[i], ans[now - 1])) <= 0) --now;
        ans[++now] = points[i];
    }
    ans.resize(now);
    return ans;
}

template <class T>
T Diameter(const std::vector<Point<T>> &points) { // 返回凸包直径的平方
    int n = int(points.size());
    if (n <= 1) return 0;
    auto distance2 = [&] (Point<T> a, Point<T> b) -> T { return Ex_Distance(a, b); };
    if (n == 2) return distance2(points[0], points[1]);
    T ans = 0;
    for (int i = 0, j = 1; i < n; ++i) {
        while (Cross(points[i], points[(i + 1) % n], points[j]) < Cross(points[i], points[(i + 1) % n], points[(j + 1) % n])) j = (j + 1) % n;
        ans = std::max(ans, distance2(points[i], points[j]));
        ans = std::max(ans, distance2(points[(i + 1) % n], points[j]));
    }
    return ans;
}

inline f64 TriangleArea(Pd a, Pd b, Pd c) { return std::abs(Cross(b - a, c - a)) / 2; } // 三角形面积

template <class T>
f64 PolygonArea(const std::vector<Point<T>> &polygon, bool absolute = true) { // 多边形面积 默认返回绝对值
    if (polygon.size() < 3) return 0;
    f64 sum = 0;
    for (int i = 0; i < int(polygon.size()); ++i) sum += f64(Cross(polygon[i], polygon[(i + 1) % polygon.size()]));
    sum /= 2;
    return absolute ? std::abs(sum) : sum;
}

inline std::pair<bool, Pd> Circumcenter(Pd a, Pd b, Pd c) { // 外心
    auto [ok, circle] = CircleFromThreePoints(a, b, c);
    return {ok, circle.O};
}

inline Pd Incenter(Pd a, Pd b, Pd c) { // 内心
    f64 wa = Distance(b, c), wb = Distance(a, c), wc = Distance(a, b);
    return (a * wa + b * wb + c * wc) / (wa + wb + wc);
}

template <class T>
Point<T> Centroid(Point<T> a, Point<T> b, Point<T> c) { return (a + b + c) / T(3); } // 三角形重心

template <class T>
int RectangleType(std::vector<Point<T>> points) { // 0: 其他 1: 矩形 2: 正方形
    static_assert(std::is_integral_v<T>);
    if (points.size() != 4) return 0;
    std::vector<T> distance;
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) distance.emplace_back(Ex_Distance(points[i], points[j]));
    }
    std::sort(distance.begin(), distance.end());
    if (distance[0] == 0 || distance[0] != distance[1] || distance[2] != distance[3] || distance[4] != distance[5] || distance[0] + distance[2] != distance[4]) return 0;
    return distance[0] == distance[2] ? 2 : 1;
}

template <class T>
int PointInPolygon(Point<T> p, const std::vector<Point<T>> &polygon) { // 0: 外部 1: 边界 2: 内部
    bool inside = false;
    int n = int(polygon.size());
    for (int i = 0; i < n; ++i) {
        Point<T> a = polygon[i], b = polygon[(i + 1) % n];
        if (Onsegment(p, Line<T>{a, b})) return 1;
        bool upward = Sign(a.y - p.y) <= 0 && Sign(b.y - p.y) > 0;
        bool downward = Sign(b.y - p.y) <= 0 && Sign(a.y - p.y) > 0;
        int side = Sign(Cross(b - a, p - a));
        if ((upward && side > 0) || (downward && side < 0)) inside = !inside;
    }
    return inside ? 2 : 0;
}

inline bool SegmentInPolygon(Ld segment, const std::vector<Pd> &polygon, bool boundaryIncluded = true) { // 线段是否完全位于简单多边形内
    int stateA = PointInPolygon(segment.a, polygon), stateB = PointInPolygon(segment.b, polygon);
    if (stateA == 0 || stateB == 0) return false;
    if (!boundaryIncluded && (stateA != 2 || stateB != 2)) return false;
    if (Distance(segment.a, segment.b) < EPS) return boundaryIncluded || stateA == 2;
    Pd direction = segment.b - segment.a;
    f64 length2 = Dot(direction, direction);
    std::vector<f64> cuts{0, 1};
    for (int i = 0; i < int(polygon.size()); ++i) {
        Ld edge{polygon[i], polygon[(i + 1) % polygon.size()]};
        auto [type, p1, p2] = SegmentIntersection(segment, edge);
        if (!boundaryIncluded && type != 0) return false;
        if (type != 0) cuts.emplace_back(Dot(p1 - segment.a, direction) / length2);
        if (type == 2) cuts.emplace_back(Dot(p2 - segment.a, direction) / length2);
    }
    std::sort(cuts.begin(), cuts.end());
    cuts.erase(std::unique(cuts.begin(), cuts.end(), [] (f64 x, f64 y) { return std::abs(x - y) < EPS; }), cuts.end());
    for (int i = 0; i + 1 < int(cuts.size()); ++i) {
        f64 t = (cuts[i] + cuts[i + 1]) / 2;
        if (PointInPolygon(segment.a + direction * t, polygon) == 0) return false;
    }
    return true;
}

inline i64 BoundaryLatticePoints(const std::vector<Pi> &polygon) { // 格点多边形边界上的格点数
    if (polygon.empty()) return 0;
    i64 ans = 0;
    for (int i = 0; i < int(polygon.size()); ++i) {
        Pi direction = polygon[(i + 1) % polygon.size()] - polygon[i];
        ans += std::gcd(std::abs(direction.x), std::abs(direction.y));
    }
    return ans;
}

inline i64 InteriorLatticePoints(const std::vector<Pi> &polygon) { // Pick 定理求内部格点数
    if (polygon.size() < 3) return 0;
    i128 area2 = 0;
    for (int i = 0; i < int(polygon.size()); ++i) {
        Pi a = polygon[i], b = polygon[(i + 1) % polygon.size()];
        area2 += i128(a.x) * b.y - i128(a.y) * b.x;
    }
    if (area2 < 0) area2 = -area2;
    i64 boundary = BoundaryLatticePoints(polygon);
    return i64((area2 - boundary + 2) / 2);
}

template <class T>
int PointInConvex(Point<T> p, const std::vector<Point<T>> &polygon) { // 0: 外部 1: 边界 2: 内部
    int n = int(polygon.size());
    if (n == 0) return 0;
    if (n == 1) return p == polygon[0] ? 1 : 0;
    if (n == 2) return Onsegment(p, Line<T>{polygon[0], polygon[1]}) ? 1 : 0;
    Point<T> base = polygon[0];
    int left = Sign(Cross(polygon[1] - base, p - base));
    int right = Sign(Cross(polygon[n - 1] - base, p - base));
    if (left < 0 || right > 0) return 0;
    if (left == 0) return Onsegment(p, Line<T>{base, polygon[1]}) ? 1 : 0;
    if (right == 0) return Onsegment(p, Line<T>{base, polygon[n - 1]}) ? 1 : 0;
    int l = 1, r = n - 1;
    while (r - l > 1) {
        int mid = (l + r) / 2;
        if (Sign(Cross(polygon[mid] - base, p - base)) >= 0) l = mid;
        else r = mid;
    }
    int side = Sign(Cross(polygon[r] - polygon[l], p - polygon[l]));
    return side < 0 ? 0 : side == 0 ? 1 : 2;
}

template <class T>
std::vector<Point<T>> MinkowskiSum(std::vector<Point<T>> a, std::vector<Point<T>> b) { // 两个凸包的闵可夫斯基和
    if (a.empty() || b.empty()) return { };
    a = Convex(std::move(a));
    b = Convex(std::move(b));
    if (a.size() <= 2 || b.size() <= 2) {
        std::vector<Point<T>> sums;
        sums.reserve(a.size() * b.size());
        for (auto x : a) for (auto y : b) sums.emplace_back(x + y);
        return Convex(std::move(sums));
    }
    auto normalize = [] (std::vector<Point<T>> &polygon) {
        auto it = std::min_element(polygon.begin(), polygon.end(), [] (Point<T> x, Point<T> y) { return x.y != y.y ? x.y < y.y : x.x < y.x; });
        std::rotate(polygon.begin(), it, polygon.end());
    };
    normalize(a);
    normalize(b);
    std::vector<Point<T>> edgeA(a.size()), edgeB(b.size());
    for (int i = 0; i < int(a.size()); ++i) edgeA[i] = a[(i + 1) % a.size()] - a[i];
    for (int i = 0; i < int(b.size()); ++i) edgeB[i] = b[(i + 1) % b.size()] - b[i];
    std::vector<Point<T>> ans{a[0] + b[0]};
    int i = 0, j = 0;
    while (i < int(edgeA.size()) || j < int(edgeB.size())) {
        Point<T> step;
        if (j == int(edgeB.size()) || (i < int(edgeA.size()) && Sign(Cross(edgeA[i], edgeB[j])) > 0)) step = edgeA[i++];
        else if (i == int(edgeA.size()) || Sign(Cross(edgeA[i], edgeB[j])) < 0) step = edgeB[j++];
        else step = edgeA[i++] + edgeB[j++];
        ans.emplace_back(ans.back() + step);
    }
    ans.pop_back();
    return Convex(std::move(ans));
}
