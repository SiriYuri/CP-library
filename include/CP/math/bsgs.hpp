#pragma once

#include "CP/math/mathbase.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <numeric>
#include <unordered_map>

inline i64 exbsgs(i64 a, i64 b, i64 mod) {
    if (mod == 1) { return 0; }
    a %= mod;
    b %= mod;
    if (a < 0) { a += mod; }
    if (b < 0) { b += mod; }
    if (b == 1) { return 0; }
    if (a == 0) { return b == 0 ? 1 : -1; }

    auto mul = [] (i64 x, i64 y, i64 mod) -> i64 {
        return i64(i128(x) * y % mod);
    };

    i64 cnt = 0, coef = 1;
    while (true) {
        i64 div = std::gcd(a, mod);
        if (div == 1) { break; }
        if (b % div) { return -1; }
        mod /= div;
        b /= div;
        coef = mul(coef, a / div, mod);
        cnt++;
        if (coef == b) { return cnt; }
    }

    struct Hash {
        size_t operator()(u64 x) const {
            static const u64 seed = std::chrono::steady_clock::now().time_since_epoch().count();
            x += seed + 0x9e3779b97f4a7c15ULL;
            x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
            x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
            return x ^ (x >> 31);
        }
    };

    i64 blk = i64(std::sqrt((long double)mod));
    while (i128(blk + 1) * (blk + 1) <= mod) blk++;
    while (i128(blk) * blk > mod) blk--;
    blk = std::max<i64>(blk, 1);

    std::unordered_map<i64, i64, Hash> baby;
    baby.reserve(size_t(blk * 2 + 1));
    i64 cur = b;
    for (i64 j = 0; j < blk; j++) {
        baby[cur] = j;
        cur = mul(cur, a, mod);
    }

    i64 step = power(a, blk, mod);
    cur = coef;
    for (i64 i = 1; i <= ceil(mod, blk); i++) {
        cur = mul(cur, step, mod);
        auto it = baby.find(cur);
        if (it != baby.end()) { return i * blk - it->second + cnt; }
    }
    return -1;
}
