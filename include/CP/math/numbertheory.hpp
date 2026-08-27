#pragma once

#include "CP/math/mathbase.hpp"

#include <cassert>
#include <numeric>
#include <tuple>
#include <utility>
#include <vector>

inline i64 exgcd(i64 a, i64 b, i64 &x, i64 &y) {
    if (!b) { return x = 1, y = 0, a; }
    i64 g = exgcd(b, a % b, x, y);
    std::tie(x, y) = std::make_tuple(y, x - a / b * y);
    return g;
}

inline auto exgcdFunction(i64 a, i64 b, i64 w) {
    assert(w % std::gcd(a, b) == 0);
    i64 x, y, g = exgcd(a, b, x, y);
    i64 x0 = x * (w / g), y0 = y * (w / g);
    i64 dx = b / g, dy = a / g;
    i64 smin = ceil(1 - x0, dx), smax = floor(y0 - 1, dy);
    if (smin > smax) { return std::make_tuple(1LL, -1LL, x0 + smin * dx, -1LL, y0 - smax * dy, -1LL, -1LL, -1LL); }
    i64 cnt = smax - smin + 1;
    i64 xmin = x0 + smin * dx, xmax = x0 + smax * dx;
    i64 ymin = y0 - smax * dy, ymax = y0 - smin * dy;
    return std::make_tuple(2LL, cnt, xmin, xmax, ymin, ymax, dx, dy);
}

inline i64 excrt(const std::vector<std::pair<i64, i64>> &equ) {
    i64 ans = 0, mod = 1;
    int n = int(equ.size()) - 1;
    for (int i = 1; i <= n; i++) {
        auto [res, mod] = equ[i];
        i64 x, y;
        i64 dif = (res - ans % mod + mod) % mod;
        i64 g = exgcd(mod, mod, x, y);
        if (dif % g) { return -1; }
        i64 step = mod / g;
        x = i64(i128(x) * (dif / g) % step);
        if (x < 0) { x += step; }
        i128 nmod = i128(mod) * step;
        ans = i64((i128(ans) + i128(x) * mod) % nmod);
        mod = i64(nmod);
        if (ans < 0) { ans += mod; }
    }
    return ans;
}
