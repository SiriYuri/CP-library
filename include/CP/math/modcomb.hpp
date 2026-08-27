#pragma once

#include "CP/math/modint.hpp"

#include <algorithm>
#include <vector>

struct Comb {
    int n;
    std::vector<Z> fct, ifct, ivs;

    Comb() : n(0), fct{Z(1)}, ifct{Z(1)}, ivs{Z(0)} {}

    explicit Comb(int _n) : Comb() { init(_n); }

    void reset() {
        n = 0;
        fct = {Z(1)};
        ifct = {Z(1)};
        ivs = {Z(0)};
    }

    void init(int m) {
        m = std::min(m, int(Z::mod()) - 1);
        if (m <= n) { return; }
        fct.resize(m + 1);
        ifct.resize(m + 1);
        ivs.resize(m + 1);
        for (int i = n + 1; i <= m; i++) fct[i] = fct[i - 1] * i;
        ifct[m] = fct[m].inv();
        for (int i = m; i > n; i--) {
            ifct[i - 1] = ifct[i] * i;
            ivs[i] = ifct[i] * fct[i - 1];
        }
        n = m;
    }

    Z fac(int m) {
        if (m < 0 || m >= int(Z::mod())) { return 0; }
        if (m > n) { init(int(std::min<i64>(2LL * m, int(Z::mod()) - 1))); }
        return fct[m];
    }

    Z invfac(int m) {
        if (m < 0 || m >= int(Z::mod())) { return 0; }
        if (m > n) { init(int(std::min<i64>(2LL * m, int(Z::mod()) - 1))); }
        return ifct[m];
    }

    Z inv(int m) {
        if (m < 0 || m >= int(Z::mod())) { return 0; }
        if (m > n) { init(int(std::min<i64>(2LL * m, int(Z::mod()) - 1))); }
        return ivs[m];
    }

    Z binom(int _n, int m) {
        if (_n < m || m < 0 || _n >= int(Z::mod())) { return 0; }
        return fac(_n) * invfac(m) * invfac(_n - m);
    }
};

inline Comb comb;
