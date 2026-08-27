#pragma once

#include "CP/geometry/circle.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <utility>
#include <vector>

template <class T>
std::vector<Point<T>> convex(std::vector<Point<T>> pts) { // 严格凸包 去重并删除边上的共线中间点 逆时针返回且不重复首点
    std::sort(pts.begin(), pts.end());
    pts.erase(std::unique(pts.begin(), pts.end()), pts.end());
    int n = int(pts.size());
    if (n <= 2) { return pts; }
    std::vector<Point<T>> ans(n * 2);
    int now = -1;
    for (int i = 0; i < n; ++i) {
        while (now > 0 && sign(cross(ans[now], pts[i], ans[now - 1])) <= 0) --now;
        ans[++now] = pts[i];
    }
    int pre = now;
    for (int i = n - 2; i >= 0; --i) {
        while (now > pre && sign(cross(ans[now], pts[i], ans[now - 1])) <= 0) --now;
        ans[++now] = pts[i];
    }
    ans.resize(now);
    return ans;
}

template <class T>
T diameter(const std::vector<Point<T>> &pts) { // 返回凸包直径的平方
    int n = int(pts.size());
    if (n <= 1) { return 0; }

    auto dis2 = [&] (Point<T> a, Point<T> b) -> T { return squaredDistance(a, b); };

    if (n == 2) { return dis2(pts[0], pts[1]); }
    T ans = 0;
    for (int i = 0, j = 1; i < n; ++i) {
        while (cross(pts[i], pts[(i + 1) % n], pts[j]) < cross(pts[i], pts[(i + 1) % n], pts[(j + 1) % n])) j = (j + 1) % n;
        ans = std::max(ans, dis2(pts[i], pts[j]));
        ans = std::max(ans, dis2(pts[(i + 1) % n], pts[j]));
    }
    return ans;
}

inline f64 triangleArea(Pd a, Pd b, Pd c) { return std::abs(cross(b - a, c - a)) / 2; } // 三角形面积

template <class T>
f64 polygonArea(const std::vector<Point<T>> &poly, bool abs = true) { // 多边形面积 默认返回绝对值
    if (poly.size() < 3) { return 0; }
    f64 sum = 0;
    for (int i = 0; i < int(poly.size()); ++i) sum += f64(cross(poly[i], poly[(i + 1) % poly.size()]));
    sum /= 2;
    return abs ? std::abs(sum) : sum;
}

inline std::pair<bool, Pd> circumcenter(Pd a, Pd b, Pd c) { // 外心
    auto [ok, circle] = circleFromThreePoints(a, b, c);
    return {ok, circle.O};
}

inline Pd incenter(Pd a, Pd b, Pd c) { // 内心
    f64 wa = distance(b, c), wb = distance(a, c), wc = distance(a, b);
    return (a * wa + b * wb + c * wc) / (wa + wb + wc);
}

template <class T>
Point<T> centroid(Point<T> a, Point<T> b, Point<T> c) { return (a + b + c) / T(3); } // 三角形重心

template <class T>
int rectangleType(std::vector<Point<T>> pts) { // 0: 其他 1: 矩形 2: 正方形
    static_assert(std::is_integral_v<T>);
    if (pts.size() != 4) { return 0; }
    std::vector<T> dis;
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) dis.emplace_back(squaredDistance(pts[i], pts[j]));
    }
    std::sort(dis.begin(), dis.end());
    if (dis[0] == 0 || dis[0] != dis[1] || dis[2] != dis[3] || dis[4] != dis[5] || dis[0] + dis[2] != dis[4]) { return 0; }
    return dis[0] == dis[2] ? 2 : 1;
}

template <class T>
int pointInPolygon(Point<T> p, const std::vector<Point<T>> &poly) { // 0: 外部 1: 边界 2: 内部
    bool inside = false;
    int n = int(poly.size());
    for (int i = 0; i < n; ++i) {
        Point<T> a = poly[i], b = poly[(i + 1) % n];
        if (onSegment(p, Line<T>{a, b})) { return 1; }
        bool upward = sign(a.y - p.y) <= 0 && sign(b.y - p.y) > 0;
        bool down = sign(b.y - p.y) <= 0 && sign(a.y - p.y) > 0;
        int side = sign(cross(b - a, p - a));
        if ((upward && side > 0) || (down && side < 0)) { inside = !inside; }
    }
    return inside ? 2 : 0;
}

inline bool segmentInPolygon(Ld seg, const std::vector<Pd> &poly, bool incl = true) { // 线段是否完全位于简单多边形内
    int sta = pointInPolygon(seg.a, poly), stb = pointInPolygon(seg.b, poly);
    if (sta == 0 || stb == 0) { return false; }
    if (!incl && (sta != 2 || stb != 2)) { return false; }
    if (distance(seg.a, seg.b) < EPS) { return incl || sta == 2; }
    Pd dir = seg.b - seg.a;
    f64 len2 = dot(dir, dir);
    std::vector<f64> cuts{0, 1};
    for (int i = 0; i < int(poly.size()); ++i) {
        Ld edge{poly[i], poly[(i + 1) % poly.size()]};
        auto [type, p1, p2] = segmentIntersection(seg, edge);
        if (!incl && type != 0) { return false; }
        if (type != 0) { cuts.emplace_back(dot(p1 - seg.a, dir) / len2); }
        if (type == 2) { cuts.emplace_back(dot(p2 - seg.a, dir) / len2); }
    }
    std::sort(cuts.begin(), cuts.end());
    cuts.erase(std::unique(cuts.begin(), cuts.end(), [] (f64 x, f64 y) { return std::abs(x - y) < EPS; }), cuts.end());
    for (int i = 0; i + 1 < int(cuts.size()); ++i) {
        f64 t = (cuts[i] + cuts[i + 1]) / 2;
        if (pointInPolygon(seg.a + dir * t, poly) == 0) { return false; }
    }
    return true;
}

inline i64 boundaryLatticePoints(const std::vector<Pi> &poly) { // 格点多边形边界上的格点数
    if (poly.empty()) { return 0; }
    i64 ans = 0;
    for (int i = 0; i < int(poly.size()); ++i) {
        Pi dir = poly[(i + 1) % poly.size()] - poly[i];
        ans += std::gcd(std::abs(dir.x), std::abs(dir.y));
    }
    return ans;
}

inline i64 interiorLatticePoints(const std::vector<Pi> &poly) { // Pick 定理求内部格点数
    if (poly.size() < 3) { return 0; }
    i128 area2 = 0;
    for (int i = 0; i < int(poly.size()); ++i) {
        Pi a = poly[i], b = poly[(i + 1) % poly.size()];
        area2 += i128(a.x) * b.y - i128(a.y) * b.x;
    }
    if (area2 < 0) { area2 = -area2; }
    i64 bound = boundaryLatticePoints(poly);
    return i64((area2 - bound + 2) / 2);
}

template <class T>
int pointInConvex(Point<T> p, const std::vector<Point<T>> &poly) { // 0: 外部 1: 边界 2: 内部
    int n = int(poly.size());
    if (n == 0) { return 0; }
    if (n == 1) { return p == poly[0] ? 1 : 0; }
    if (n == 2) { return onSegment(p, Line<T>{poly[0], poly[1]}) ? 1 : 0; }
    Point<T> base = poly[0];
    int left = sign(cross(poly[1] - base, p - base));
    int rgt = sign(cross(poly[n - 1] - base, p - base));
    if (left < 0 || rgt > 0) { return 0; }
    if (left == 0) { return onSegment(p, Line<T>{base, poly[1]}) ? 1 : 0; }
    if (rgt == 0) { return onSegment(p, Line<T>{base, poly[n - 1]}) ? 1 : 0; }
    int l = 1, r = n - 1;
    while (r - l > 1) {
        int mid = (l + r) / 2;
        if (sign(cross(poly[mid] - base, p - base)) >= 0) { l = mid; }
        else r = mid;
    }
    int side = sign(cross(poly[r] - poly[l], p - poly[l]));
    return side < 0 ? 0 : side == 0 ? 1 : 2;
}

template <class T>
std::vector<Point<T>> minkowskiSum(std::vector<Point<T>> a, std::vector<Point<T>> b) { // 两个凸包的闵可夫斯基和
    if (a.empty() || b.empty()) { return { }; }
    a = convex(std::move(a));
    b = convex(std::move(b));
    if (a.size() <= 2 || b.size() <= 2) {
        std::vector<Point<T>> sums;
        sums.reserve(a.size() * b.size());
        for (auto x : a) for (auto y : b) sums.emplace_back(x + y);
        return convex(std::move(sums));
    }
    auto norm = [] (std::vector<Point<T>> &poly) {
        auto it = std::min_element(poly.begin(), poly.end(), [] (Point<T> x, Point<T> y) { return x.y != y.y ? x.y < y.y : x.x < y.x; });
        std::rotate(poly.begin(), it, poly.end());
    };

    norm(a);
    norm(b);
    std::vector<Point<T>> eda(a.size()), edb(b.size());
    for (int i = 0; i < int(a.size()); ++i) eda[i] = a[(i + 1) % a.size()] - a[i];
    for (int i = 0; i < int(b.size()); ++i) edb[i] = b[(i + 1) % b.size()] - b[i];
    std::vector<Point<T>> ans{a[0] + b[0]};
    int i = 0, j = 0;
    while (i < int(eda.size()) || j < int(edb.size())) {
        Point<T> step;
        if (j == int(edb.size()) || (i < int(eda.size()) && sign(cross(eda[i], edb[j])) > 0)) { step = eda[i++]; }
        else if (i == int(eda.size()) || sign(cross(eda[i], edb[j])) < 0) step = edb[j++];
        else step = eda[i++] + edb[j++];
        ans.emplace_back(ans.back() + step);
    }
    ans.pop_back();
    return convex(std::move(ans));
}
