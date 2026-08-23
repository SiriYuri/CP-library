#pragma once

#include "CP/geometry/line.hpp"

#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>

inline bool InHalfPlane(Pd p, Ld line, bool boundaryIncluded = true) { // 有向直线左侧的半平面
    int side = Sign(Cross(line.b - line.a, p - line.a));
    return boundaryIncluded ? side >= 0 : side > 0;
}

inline std::vector<Pd> HalfPlaneIntersection(std::vector<Ld> lines) { // 半平面交 空集或无界交返回空
    lines.erase(std::remove_if(lines.begin(), lines.end(), [] (Ld line) { return Distance(line.a, line.b) < EPS; }), lines.end());
    if (lines.empty()) return { };
    auto angle = [] (Ld line) {
        Pd direction = line.b - line.a;
        return std::atan2(direction.y, direction.x);
    };
    std::sort(lines.begin(), lines.end(), [&] (Ld l1, Ld l2) {
        f64 angle1 = angle(l1), angle2 = angle(l2);
        if (angle1 != angle2) return angle1 < angle2;
        return Sign(Cross(l1.b - l1.a, l2.a - l1.a)) > 0;
    });
    std::vector<Ld> uniqueLines;
    for (Ld line : lines) {
        if (!uniqueLines.empty()) {
            Pd d1 = uniqueLines.back().b - uniqueLines.back().a;
            Pd d2 = line.b - line.a;
            if (Sign(Cross(d1, d2)) == 0 && Sign(Dot(d1, d2)) > 0) {
                if (InHalfPlane(line.a, uniqueLines.back())) uniqueLines.back() = line;
                continue;
            }
        }
        uniqueLines.emplace_back(line);
    }
    std::deque<Ld> que;
    std::deque<Pd> points;
    for (Ld line : uniqueLines) {
        while (!points.empty() && !InHalfPlane(points.back(), line)) points.pop_back(), que.pop_back();
        while (!points.empty() && !InHalfPlane(points.front(), line)) points.pop_front(), que.pop_front();
        if (!que.empty()) {
            if (Parallel(que.back(), line)) return { };
            points.emplace_back(LineIntersection(que.back(), line));
        }
        que.emplace_back(line);
    }
    while (!points.empty() && !InHalfPlane(points.back(), que.front())) points.pop_back(), que.pop_back();
    while (!points.empty() && !InHalfPlane(points.front(), que.back())) points.pop_front(), que.pop_front();
    if (que.size() < 3 || Parallel(que.back(), que.front())) return { };
    points.emplace_back(LineIntersection(que.back(), que.front()));
    return {points.begin(), points.end()};
}
