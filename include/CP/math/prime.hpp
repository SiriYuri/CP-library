#pragma once

#include "CP/types.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <numeric>
#include <random>
#include <vector>

namespace PrimeDetail {
inline u64 multiply(u64 a, u64 b, u64 mod) {
    return u64(u128(a) * b % mod);
}

inline u64 power(u64 x, u64 exp, u64 mod) {
    u64 ans = 1;
    for (; exp; exp >>= 1, x = multiply(x, x, mod)) {
        if (exp & 1) ans = multiply(ans, x, mod);
    }
    return ans;
}

inline bool isPrime(u64 n) {
    static constexpr std::array<u32, 12> small {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (u64 p : small) {
        if (n % p == 0) return n == p;
    }
    if (n < 2) return false;

    u64 d = n - 1;
    int s = std::countr_zero(d);
    d >>= s;
    static constexpr std::array<u64, 7> bases {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    for (u64 a : bases) {
        if (a % n == 0) continue;
        u64 x = power(a % n, d, n);
        if (x == 1 || x == n - 1) continue;
        bool composite = true;
        for (int r = 1; r < s; r++) {
            x = multiply(x, x, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        if (composite) return false;
    }
    return true;
}

inline u64 random() {
    static std::mt19937_64 engine(std::chrono::steady_clock::now().time_since_epoch().count());
    return engine();
}

inline u64 findFactor(u64 n) {
    if (n % 2 == 0) return 2;
    if (n % 3 == 0) return 3;
    while (true) {
        u64 y = random() % (n - 1) + 1;
        u64 c = random() % (n - 1) + 1;
        auto next = [=] (u64 x) {
            return u64((u128(x) * x + c) % n);
        };

        u64 x = 0, ys = 0, q = 1;
        u64 g = 1, r = 1;
        while (g == 1) {
            x = y;
            for (u64 i = 0; i < r; i++) y = next(y);
            for (u64 k = 0; k < r && g == 1; k += 128) {
                ys = y;
                q = 1;
                u64 limit = std::min<u64>(128, r - k);
                for (u64 i = 0; i < limit; i++) {
                    y = next(y);
                    q = multiply(q, x > y ? x - y : y - x, n);
                }
                g = std::gcd(q, n);
            }
            r <<= 1;
        }
        if (g == n) {
            do {
                ys = next(ys);
                g = std::gcd(x > ys ? x - ys : ys - x, n);
            } while (g == 1);
        }
        if (g != n) return g;
    }
}

inline void factor(u64 n, std::vector<u64> &result) {
    if (n == 1) return;
    if (isPrime(n)) {
        result.emplace_back(n);
        return;
    }
    u64 divisor = findFactor(n);
    factor(divisor, result);
    factor(n / divisor, result);
}
}

inline bool MillerRabin(u64 n) {
    return PrimeDetail::isPrime(n);
}

inline u64 PollardRho(u64 n) {
    if (n < 2 || MillerRabin(n)) return n;
    return PrimeDetail::findFactor(n);
}

inline std::vector<u64> factorize(u64 n) {
    std::vector<u64> result;
    if (n > 1) PrimeDetail::factor(n, result);
    std::ranges::sort(result);
    return result;
}
