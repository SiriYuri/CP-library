#pragma once

#include "CP/types.hpp"

#include <cassert>
#include <compare>
#include <concepts>
#include <iostream>
#include <utility>

template <class T>
constexpr T power(T base, i64 exp) {
    T ans = 1;
    for (; exp > 0; exp >>= 1, base *= base) {
        if (exp & 1) ans *= base;
    }
    return ans;
}

struct Montgomery {
    u32 m;
    u32 ninv;
    u32 r2;

    constexpr explicit Montgomery(u32 m_ = 1) : m(m_) {
        assert((m_ & 1U) && m_ < (1U << 31));
        u32 inv = 1;
        for (int i = 0; i < 5; ++i) {
            inv *= 2 - m_ * inv;
        }
        ninv = -inv;

        const u64 r = (1ULL << 32) % m_;
        r2 = u32((r * r) % m_);
    }

    constexpr u32 reduce(u64 value) const {
        const u32 m_value = u32(value) * ninv;
        const u64 reduced = value + u64(m_value) * m;
        const u32 res = u32(reduced >> 32);
        return res >= m ? res - m : res;
    }

    constexpr u32 mul(u32 a, u32 b) const {
        return reduce(u64(a) * b);
    }

    constexpr u32 to_mont(u32 a) const {
        return reduce(u64(a) * r2);
    }

    constexpr u32 from_mont(u32 a) const {
        return reduce(a);
    }
};

template <u32 P>
struct DynModInt {
private:
    u32 x;
    static inline Montgomery mt{P > 0 ? P : 1};

public:
    static constexpr u32 default_mod = P;

    constexpr DynModInt() : x(0) {}

    template <std::integral T>
    constexpr DynModInt(T value) {
        i64 normalized = value % i64(mod());
        if (normalized < 0) {
            normalized += mod();
        }
        x = mt.to_mont(u32(normalized));
    }

    static void setMod(u32 modulus) {
        assert((modulus & 1U) && modulus < (1U << 31));
        mt = Montgomery(modulus);
    }

    static constexpr u32 mod() {
        return mt.m;
    }

    constexpr u32 val() const {
        return mt.from_mont(x);
    }

    constexpr DynModInt &operator+=(const DynModInt &rhs) & {
        x += rhs.x;
        if (x >= mod()) {
            x -= mod();
        }
        return *this;
    }

    constexpr DynModInt &operator-=(const DynModInt &rhs) & {
        x = x >= rhs.x ? x - rhs.x : x + mod() - rhs.x;
        return *this;
    }

    constexpr DynModInt &operator*=(const DynModInt &rhs) & {
        x = mt.mul(x, rhs.x);
        return *this;
    }

    constexpr DynModInt &operator/=(const DynModInt &rhs) & {
        return *this *= rhs.inv();
    }

    constexpr DynModInt &operator++() & {
        return *this += 1;
    }

    constexpr DynModInt operator++(int) & {
        DynModInt tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr DynModInt &operator--() & {
        return *this -= 1;
    }

    constexpr DynModInt operator--(int) & {
        DynModInt tmp = *this;
        --*this;
        return tmp;
    }

    constexpr DynModInt inv() const {
        const u32 regular = mt.from_mont(x);
        i64 a = regular;
        i64 b = mod();
        i64 u = 1;
        i64 v = 0;
        while (b) {
            const i64 quotient = a / b;
            a -= quotient * b;
            std::swap(a, b);
            u -= quotient * v;
            std::swap(u, v);
        }
        assert(a == 1);
        return DynModInt(u);
    }

    constexpr DynModInt operator-() const {
        DynModInt res;
        res.x = x == 0 ? 0 : mod() - x;
        return res;
    }

    friend constexpr bool operator==(const DynModInt &, const DynModInt &) = default;

    friend constexpr auto operator<=>(const DynModInt &lhs, const DynModInt &rhs) {
        return lhs.val() <=> rhs.val();
    }

    friend constexpr DynModInt operator+(DynModInt lhs, const DynModInt &rhs) {
        return lhs += rhs;
    }

    friend constexpr DynModInt operator-(DynModInt lhs, const DynModInt &rhs) {
        return lhs -= rhs;
    }

    friend constexpr DynModInt operator*(DynModInt lhs, const DynModInt &rhs) {
        return lhs *= rhs;
    }

    friend constexpr DynModInt operator/(DynModInt lhs, const DynModInt &rhs) {
        return lhs /= rhs;
    }

    friend std::istream &operator>>(std::istream &is, DynModInt &value) {
        i64 input;
        is >> input;
        value = input;
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const DynModInt &value) {
        return os << value.val();
    }
};

constexpr u32 P = 998'244'353;
using Z = DynModInt<P>;
