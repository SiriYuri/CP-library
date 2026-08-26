#pragma once

#include <cstdint>
#include <limits>

using i32 = int;
using i64 = long long;
using i128 = __int128_t;

using u32 = unsigned int;
using u64 = unsigned long long;
using u128 = __uint128_t;

using f32 = double;
using f64 = long double;

inline constexpr f64 EPS = 1e-8;

template <typename T>
inline constexpr T inf = std::numeric_limits<T>::max() >> 2;
