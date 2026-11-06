#pragma once

#include "CP/math/modint.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

inline std::vector<int> rev;

template <u32 P>
inline std::vector<DynModInt<P>> rts{0, 1};

template <u32 P>
DynModInt<P> findPrimitiveRoot() {
    DynModInt<P> val = 2;
    int k = __builtin_ctz(DynModInt<P>::mod() - 1);
    while (power(val, (DynModInt<P>::mod() - 1) / 2) == 1) val += 1;
    return power(val, (DynModInt<P>::mod() - 1) >> k);
}

template <u32 P>
inline DynModInt<P> proot = findPrimitiveRoot<P>();

template <>
inline DynModInt<998244353> proot<998244353>{31};

template <u32 P>
void dft(std::vector<DynModInt<P>> &a) {
    int n = int(a.size());
    if (n <= 1) { return; }
    assert((n & (n - 1)) == 0);
    assert((DynModInt<P>::mod() - 1) % n == 0);
    if (int(rev.size()) != n) {
        int k = __builtin_ctz(n) - 1;
        rev.resize(n);
        for (int i = 0; i < n; i++) rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << k);
    }
    for (int i = 0; i < n; i++) {
        if (rev[i] < i) { std::swap(a[i], a[rev[i]]); }
    }
    if (int(rts<P>.size()) < n) {
        int k = __builtin_ctz(int(rts<P>.size()));
        rts<P>.resize(n);
        while ((1 << k) < n) {
            auto e = power(proot<P>, 1 << (__builtin_ctz(DynModInt<P>::mod() - 1) - k - 1));
            for (int i = 1 << (k - 1); i < (1 << k); i++) {
                rts<P>[2 * i] = rts<P>[i];
                rts<P>[2 * i + 1] = rts<P>[i] * e;
            }
            k++;
        }
    }
    for (int k = 1; k < n; k <<= 1) {
        for (int i = 0; i < n; i += 2 * k) {
            for (int j = 0; j < k; j++) {
                DynModInt<P> u = a[i + j];
                DynModInt<P> v = a[i + j + k] * rts<P>[k + j];
                a[i + j] = u + v;
                a[i + j + k] = u - v;
            }
        }
    }
}

template <u32 P>
void idft(std::vector<DynModInt<P>> &a) {
    int n = int(a.size());
    std::reverse(a.begin() + 1, a.end());
    dft(a);
    DynModInt<P> ivs = (1 - i64(DynModInt<P>::mod())) / n;
    for (auto &val : a) val *= ivs;
}

struct Poly : public std::vector<Z> {
    using Value = Z;

    Poly() = default;
    explicit Poly(int n) : std::vector<Value>(n) {}

    Poly(int n, const Value &val) : std::vector<Value>(n, val) {}

    Poly(const std::vector<Value> &vals) : std::vector<Value>(vals) {}

    Poly(std::vector<Value> &&vals) : std::vector<Value>(std::move(vals)) {}

    Poly(const std::initializer_list<Value> &vals) : std::vector<Value>(vals) {}

    template <class InputIt, std::enable_if_t<!std::is_integral_v<InputIt>, int> = 0>
    Poly(InputIt first, InputIt last) : std::vector<Value>(first, last) {}

    template <class F, std::enable_if_t<std::is_invocable_v<F, int>, int> = 0>
    explicit Poly(int n, F func) : std::vector<Value>(n) {
        for (int i = 0; i < n; i++) (*this)[i] = func(i);
    }

    Poly &operator=(const std::vector<Value> &vals) {
        std::vector<Value>::operator=(vals);
        return *this;
    }

    Poly &operator=(std::vector<Value> &&vals) {
        std::vector<Value>::operator=(std::move(vals));
        return *this;
    }

    Poly shift(int k) const {
        if (k >= 0) {
            Poly res = *this;
            res.insert(res.begin(), k, 0);
            return res;
        }
        if (int(this->size()) <= -k) { return {}; }
        return Poly(this->begin() - k, this->end());
    }

    Poly trunc(int k) const {
        Poly res = *this;
        res.resize(k);
        return res;
    }

    friend Poly operator+(const Poly &a, const Poly &b) {
        Poly res(std::max(a.size(), b.size()));
        for (int i = 0; i < int(a.size()); i++) res[i] += a[i];
        for (int i = 0; i < int(b.size()); i++) res[i] += b[i];
        return res;
    }

    friend Poly operator-(const Poly &a, const Poly &b) {
        Poly res(std::max(a.size(), b.size()));
        for (int i = 0; i < int(a.size()); i++) res[i] += a[i];
        for (int i = 0; i < int(b.size()); i++) res[i] -= b[i];
        return res;
    }

    friend Poly operator-(const Poly &val) {
        Poly res(val.size());
        for (int i = 0; i < int(val.size()); i++) res[i] = -val[i];
        return res;
    }

    friend Poly operator*(Poly a, Poly b) {
        if (a.empty() || b.empty()) { return {}; }
        if (a.size() < b.size()) { std::swap(a, b); }
        int tot = int(a.size() + b.size()) - 1, n = 1;
        while (n < tot) n <<= 1;
        if (((Value::mod() - 1) & (n - 1)) != 0 || b.size() < 128) {
            Poly res(tot);
            for (int i = 0; i < int(a.size()); i++) {
                for (int j = 0; j < int(b.size()); j++) res[i + j] += a[i] * b[j];
            }
            return res;
        }
        a.resize(n);
        b.resize(n);
        dft<P>(a);
        dft<P>(b);
        for (int i = 0; i < n; i++) a[i] *= b[i];
        idft<P>(a);
        a.resize(tot);
        return a;
    }

    friend Poly operator*(Value val, Poly poly) {
        for (auto &coef : poly) coef *= val;
        return poly;
    }

    friend Poly operator*(Poly poly, Value val) { return val * poly; }

    friend Poly operator/(Poly poly, Value val) {
        for (auto &coef : poly) coef /= val;
        return poly;
    }

    Poly &operator+=(const Poly &rhs) { return *this = *this + rhs; }

    Poly &operator-=(const Poly &rhs) { return *this = *this - rhs; }

    Poly &operator*=(const Poly &rhs) { return *this = *this * rhs; }

    Poly &operator*=(Value rhs) { return *this = *this * rhs; }

    Poly &operator/=(Value rhs) { return *this = *this / rhs; }

    Poly deriv() const {
        if (this->empty()) { return {}; }
        Poly res(this->size() - 1);
        for (int i = 0; i + 1 < int(this->size()); i++) res[i] = (i + 1) * (*this)[i + 1];
        return res;
    }

    Poly integr() const {
        Poly res(this->size() + 1);
        for (int i = 0; i < int(this->size()); i++) res[i + 1] = (*this)[i] / (i + 1);
        return res;
    }

    Poly inv(int m) const {
        Poly res{this->empty() ? Value(0) : (*this)[0].inv()};
        int k = 1;
        while (k < m) {
            k <<= 1;
            res = (res * (Poly{2} - trunc(k) * res)).trunc(k);
        }
        return res.trunc(m);
    }

    Poly log(int m) const { return (deriv() * inv(m)).integr().trunc(m); }

    Poly exp(int m) const {
        Poly res{1};
        int k = 1;
        while (k < m) {
            k <<= 1;
            res = (res * (Poly{1} - res.log(k) + trunc(k))).trunc(k);
        }
        return res.trunc(m);
    }

    Poly pow(int k, int m) const {
        int first = 0;
        while (first < int(this->size()) && (*this)[first] == Value(0)) first++;
        if (first == int(this->size()) || i64(first) * k >= m) { return Poly(m); }
        Value val = (*this)[first];
        Poly norm = shift(-first) * val.inv();
        return (norm.log(m - first * k) * k).exp(m - first * k).shift(first * k) * power(val, k);
    }

    Poly sqrt(int m) const {
        assert(!this->empty() && (*this)[0] == Value(1));
        Poly res{1};
        int k = 1;
        Value inv2 = Value(2).inv();
        while (k < m) {
            k <<= 1;
            res = (res + (trunc(k) * res.inv(k)).trunc(k)) * inv2;
        }
        return res.trunc(m);
    }

    Poly mulT(Poly b) const {
        if (b.empty()) { return {}; }
        int n = int(b.size());
        std::reverse(b.begin(), b.end());
        return (*this * b).shift(-(n - 1));
    }

    std::vector<Value> eval(std::vector<Value> pts) const {
        if (this->empty()) { return std::vector<Value>(pts.size()); }
        int n = int(std::max(pts.size(), this->size()));
        std::vector<Poly> prod(4 * n);
        std::vector<Value> ans(pts.size());
        pts.resize(n);
        auto build = [&] (auto &&self, int p, int l, int r) -> void {
            if (r - l == 1) {
                prod[p] = Poly{1, -pts[l]};
                return;
            }
            int mid = (l + r) >> 1;
            self(self, p << 1, l, mid);
            self(self, p << 1 | 1, mid, r);
            prod[p] = prod[p << 1] * prod[p << 1 | 1];
        };

        build(build, 1, 0, n);
        auto work = [&] (auto &&self, int p, int l, int r, const Poly &val) -> void {
            if (r - l == 1) {
                if (l < int(ans.size())) { ans[l] = val[0]; }
                return;
            }
            int mid = (l + r) >> 1;
            self(self, p << 1, l, mid, val.mulT(prod[p << 1 | 1]).trunc(mid - l));
            self(self, p << 1 | 1, mid, r, val.mulT(prod[p << 1]).trunc(r - mid));
        };

        work(work, 1, 0, n, mulT(prod[1].inv(n)));
        return ans;
    }
};
