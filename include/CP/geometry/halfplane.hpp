#pragma once

#include "CP/geometry/line.hpp"

#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>

inline bool inHalfPlane(Pd p, Ld line, bool incl = true) { // 有向直线左侧的半平面
    int side = sign(cross(line.b - line.a, p - line.a));
    return incl ? side >= 0 : side > 0;
}

inline std::vector<Pd> halfPlaneIntersection(std::vector<Ld> lns) { // 半平面交 空集或无界交返回空
    lns.erase(std::remove_if(lns.begin(), lns.end(), [] (Ld line) { return distance(line.a, line.b) < EPS; }), lns.end());
    if (lns.empty()) { return { }; }

    auto ang = [] (Ld line) {
        Pd dir = line.b - line.a;
        return std::atan2(dir.y, dir.x);
    };

    std::sort(lns.begin(), lns.end(), [&] (Ld l1, Ld l2) {
        f64 angle1 = ang(l1), angle2 = ang(l2);
        if (angle1 != angle2) { return angle1 < angle2; }
        return sign(cross(l1.b - l1.a, l2.a - l1.a)) > 0;
    });
    std::vector<Ld> ulns;
    for (Ld line : lns) {
        if (!ulns.empty()) {
            Pd d1 = ulns.back().b - ulns.back().a;
            Pd d2 = line.b - line.a;
            if (sign(cross(d1, d2)) == 0 && sign(dot(d1, d2)) > 0) {
                if (inHalfPlane(line.a, ulns.back())) { ulns.back() = line; }
                continue;
            }
        }
        ulns.emplace_back(line);
    }
    std::deque<Ld> que;
    std::deque<Pd> pts;
    for (Ld line : ulns) {
        while (!pts.empty() && !inHalfPlane(pts.back(), line)) pts.pop_back(), que.pop_back();
        while (!pts.empty() && !inHalfPlane(pts.front(), line)) pts.pop_front(), que.pop_front();
        if (!que.empty()) {
            if (parallel(que.back(), line)) { return { }; }
            pts.emplace_back(lineIntersection(que.back(), line));
        }
        que.emplace_back(line);
    }
    while (!pts.empty() && !inHalfPlane(pts.back(), que.front())) pts.pop_back(), que.pop_back();
    while (!pts.empty() && !inHalfPlane(pts.front(), que.back())) pts.pop_front(), que.pop_front();
    if (que.size() < 3 || parallel(que.back(), que.front())) { return { }; }
    pts.emplace_back(lineIntersection(que.back(), que.front()));
    return {pts.begin(), pts.end()};
}
