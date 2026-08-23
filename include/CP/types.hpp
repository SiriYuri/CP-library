#pragma once

#include <cstdint>
#include <limits>

using i32 = std::int32_t;
using i64 = std::int64_t;
using i128 = __int128_t;

using u32 = std::uint32_t;
using u64 = std::uint64_t;
using u128 = __uint128_t;

using f32 = double;
using f64 = long double;

inline constexpr f64 EPS = 1e-8;

template <typename T>
inline constexpr T inf = std::numeric_limits<T>::max() >> 2;
