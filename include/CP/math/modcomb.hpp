#pragma once

#include "CP/math/modint.hpp"

#include <algorithm>
#include <vector>

struct Comb {
    int n;
    std::vector<Z> factorial, inverseFactorial, inverse;

    Comb() : n(0), factorial{Z(1)}, inverseFactorial{Z(1)}, inverse{Z(0)} {}

    explicit Comb(int _n) : Comb() { init(_n); }

    void reset() {
        n = 0;
        factorial = {Z(1)};
        inverseFactorial = {Z(1)};
        inverse = {Z(0)};
    }

    void init(int m) {
        m = std::min(m, int(Z::mod()) - 1);
        if (m <= n) return;
        factorial.resize(m + 1);
        inverseFactorial.resize(m + 1);
        inverse.resize(m + 1);
        for (int i = n + 1; i <= m; i++) factorial[i] = factorial[i - 1] * i;
        inverseFactorial[m] = factorial[m].inv();
        for (int i = m; i > n; i--) {
            inverseFactorial[i - 1] = inverseFactorial[i] * i;
            inverse[i] = inverseFactorial[i] * factorial[i - 1];
        }
        n = m;
    }

    Z fac(int m) {
        if (m < 0 || m >= int(Z::mod())) return 0;
        if (m > n) init(int(std::min<i64>(2LL * m, int(Z::mod()) - 1)));
        return factorial[m];
    }

    Z invfac(int m) {
        if (m < 0 || m >= int(Z::mod())) return 0;
        if (m > n) init(int(std::min<i64>(2LL * m, int(Z::mod()) - 1)));
        return inverseFactorial[m];
    }

    Z inv(int m) {
        if (m < 0 || m >= int(Z::mod())) return 0;
        if (m > n) init(int(std::min<i64>(2LL * m, int(Z::mod()) - 1)));
        return inverse[m];
    }

    Z binom(int _n, int m) {
        if (_n < m || m < 0 || _n >= int(Z::mod())) return 0;
        return fac(_n) * invfac(m) * invfac(_n - m);
    }
};

inline Comb comb;
