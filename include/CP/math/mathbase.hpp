#pragma once

#include "CP/types.hpp"

inline i64 power(i64 x, i64 exp, i64 mod) {
    x %= mod;
    if (x < 0) { x += mod; }
    i64 ans = 1 % mod;
    for (; exp > 0; exp >>= 1, x = i64(i128(x) * x % mod)) {
        if (exp & 1) { ans = i64(i128(ans) * x % mod); }
    }
    return ans;
}

template <typename T, typename U>
constexpr auto floor(T a, U b) {
    using R = decltype(a / b);
    R quo = a / b, rem = a % b;
    return quo - (rem != 0 && (rem < 0) != (b < 0));
}

template <typename T, typename U>
constexpr auto ceil(T a, U b) {
    using R = decltype(a / b);
    R quo = a / b, rem = a % b;
    return quo + (rem != 0 && (rem < 0) == (b < 0));
}
