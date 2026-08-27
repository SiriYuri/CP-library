#pragma once

#include <cctype>
#include <cstdio>
#include <sstream>
#include <string>
#include <type_traits>

namespace IOS {
inline constexpr int BSIZE = 1 << 20; inline char buf[BSIZE]; inline int bi = BSIZE, bn = BSIZE;
inline int gc() { if (bi >= bn) { bn = int(std::fread(buf, 1, BSIZE, stdin)); bi = 0; if (!bn) { return EOF; } } return buf[bi++]; }

struct io {
    inline void read(char &c) const {
        int ch = gc();
        while (ch != EOF && std::isspace(static_cast<unsigned char>(ch))) ch = gc();
        c = ch == EOF ? '\0' : char(ch);
    }

    inline void read(std::string &s) const {
        int ch = gc();
        while (ch != EOF && std::isspace(static_cast<unsigned char>(ch))) ch = gc();
        s.clear();
        while (ch != EOF && !std::isspace(static_cast<unsigned char>(ch))) s += char(ch), ch = gc();
    }

    inline void read(char *s) const {
        int ch = gc();
        while (ch != EOF && std::isspace(static_cast<unsigned char>(ch))) ch = gc();
        while (ch != EOF && !std::isspace(static_cast<unsigned char>(ch))) *s++ = char(ch), ch = gc();
        *s = '\0';
    }

    template <typename T>
    inline void read(T &x) const {
        x = 0; bool neg = false; int ch = gc();
        while (ch != EOF && ch != '-' && (ch < '0' || ch > '9')) ch = gc();
        if (ch == '-') { neg = true, ch = gc(); }
        while (ch >= '0' && ch <= '9') x = x * 10 + (ch ^ 48), ch = gc();
        if constexpr (std::is_floating_point_v<T>) {
            if (ch == '.') {
                T p = 0.1; ch = gc();
                while (ch >= '0' && ch <= '9') x += p * (ch ^ 48), ch = gc(), p *= 0.1;
            }
        }
        if (neg) { x = -x; }
    }

    template <typename T>
    const io &operator >> (T &x) const {
        read(x); return *this;
    }

    inline void print(char c) const {
        std::putchar(c);
    }

    inline void print(const std::string &s) const {
        for (char c : s) std::putchar(c);
    }

    inline void print(const char *s) const {
        while (*s) std::putchar(*s++);
    }

    template <typename T>
    inline void printInteger(T x) const {
        if (x == 0) { std::putchar('0'); return ; }
        using U = std::make_unsigned_t<T>; U val;
        if constexpr (std::is_signed_v<T>) {
            if (x < 0) { std::putchar('-'), val = U(0) - U(x); }
            else val = U(x);
        } else val = x;
        char s[40]; int p = 0;
        while (val) s[p++] = char('0' + val % 10), val /= 10;
        while (p--) std::putchar(s[p]);
    }

    template <typename T>
    inline void print(T x) const {
        if constexpr (std::is_integral_v<T>) { printInteger(x); }
        else if constexpr (std::is_floating_point_v<T>) { std::ostringstream oss; oss << x; print(oss.str()); }
    }

    template <typename T>
    const io &operator << (const T &x) const { print(x); return *this; }

    const io &operator << (const io &(*man)(const io &)) const { return man(*this); }
};

inline const io &endl(const io &stm) { std::putchar('\n'); return stm; }

inline const io IO;
inline const io &cin = IO;
inline const io &cout = IO;
}
