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
inline std::vector<DynModInt<P>> roots{0, 1};

template <u32 P>
DynModInt<P> findPrimitiveRoot() {
    DynModInt<P> value = 2;
    int k = __builtin_ctz(DynModInt<P>::mod() - 1);
    while (power(value, (DynModInt<P>::mod() - 1) / 2) == 1) value += 1;
    return power(value, (DynModInt<P>::mod() - 1) >> k);
}

template <u32 P>
inline DynModInt<P> primitiveRoot = findPrimitiveRoot<P>();

template <>
inline DynModInt<998244353> primitiveRoot<998244353>{31};

template <u32 P>
void dft(std::vector<DynModInt<P>> &a) {
    int n = int(a.size());
    if (n <= 1) return;
    assert((n & (n - 1)) == 0);
    assert((DynModInt<P>::mod() - 1) % n == 0);
    if (int(rev.size()) != n) {
        int k = __builtin_ctz(n) - 1;
        rev.resize(n);
        for (int i = 0; i < n; i++) rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << k);
    }
    for (int i = 0; i < n; i++) {
        if (rev[i] < i) std::swap(a[i], a[rev[i]]);
    }
    if (int(roots<P>.size()) < n) {
        int k = __builtin_ctz(int(roots<P>.size()));
        roots<P>.resize(n);
        while ((1 << k) < n) {
            auto e = power(primitiveRoot<P>, 1 << (__builtin_ctz(DynModInt<P>::mod() - 1) - k - 1));
            for (int i = 1 << (k - 1); i < (1 << k); i++) {
                roots<P>[2 * i] = roots<P>[i];
                roots<P>[2 * i + 1] = roots<P>[i] * e;
            }
            k++;
        }
    }
    for (int k = 1; k < n; k <<= 1) {
        for (int i = 0; i < n; i += 2 * k) {
            for (int j = 0; j < k; j++) {
                DynModInt<P> u = a[i + j];
                DynModInt<P> v = a[i + j + k] * roots<P>[k + j];
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
    DynModInt<P> inverse = (1 - i64(DynModInt<P>::mod())) / n;
    for (auto &value : a) value *= inverse;
}

struct Poly : public std::vector<Z> {
    using Value = Z;

    Poly() = default;
    explicit Poly(int n) : std::vector<Value>(n) {}
    Poly(int n, const Value &value) : std::vector<Value>(n, value) {}
    Poly(const std::vector<Value> &values) : std::vector<Value>(values) {}
    Poly(std::vector<Value> &&values) : std::vector<Value>(std::move(values)) {}
    Poly(const std::initializer_list<Value> &values) : std::vector<Value>(values) {}

    template <class InputIt, std::enable_if_t<!std::is_integral_v<InputIt>, int> = 0>
    Poly(InputIt first, InputIt last) : std::vector<Value>(first, last) {}

    template <class F, std::enable_if_t<std::is_invocable_v<F, int>, int> = 0>
    explicit Poly(int n, F function) : std::vector<Value>(n) {
        for (int i = 0; i < n; i++) (*this)[i] = function(i);
    }

    Poly &operator=(const std::vector<Value> &values) {
        std::vector<Value>::operator=(values);
        return *this;
    }

    Poly &operator=(std::vector<Value> &&values) {
        std::vector<Value>::operator=(std::move(values));
        return *this;
    }

    Poly shift(int k) const {
        if (k >= 0) {
            Poly result = *this;
            result.insert(result.begin(), k, 0);
            return result;
        }
        if (int(this->size()) <= -k) return {};
        return Poly(this->begin() - k, this->end());
    }

    Poly trunc(int k) const {
        Poly result = *this;
        result.resize(k);
        return result;
    }

    friend Poly operator+(const Poly &a, const Poly &b) {
        Poly result(std::max(a.size(), b.size()));
        for (int i = 0; i < int(a.size()); i++) result[i] += a[i];
        for (int i = 0; i < int(b.size()); i++) result[i] += b[i];
        return result;
    }

    friend Poly operator-(const Poly &a, const Poly &b) {
        Poly result(std::max(a.size(), b.size()));
        for (int i = 0; i < int(a.size()); i++) result[i] += a[i];
        for (int i = 0; i < int(b.size()); i++) result[i] -= b[i];
        return result;
    }

    friend Poly operator-(const Poly &value) {
        Poly result(value.size());
        for (int i = 0; i < int(value.size()); i++) result[i] = -value[i];
        return result;
    }

    friend Poly operator*(Poly a, Poly b) {
        if (a.empty() || b.empty()) return {};
        if (a.size() < b.size()) std::swap(a, b);
        int total = int(a.size() + b.size()) - 1, n = 1;
        while (n < total) n <<= 1;
        if (((Value::mod() - 1) & (n - 1)) != 0 || b.size() < 128) {
            Poly result(total);
            for (int i = 0; i < int(a.size()); i++) {
                for (int j = 0; j < int(b.size()); j++) result[i + j] += a[i] * b[j];
            }
            return result;
        }
        a.resize(n);
        b.resize(n);
        dft<P>(a);
        dft<P>(b);
        for (int i = 0; i < n; i++) a[i] *= b[i];
        idft<P>(a);
        a.resize(total);
        return a;
    }

    friend Poly operator*(Value value, Poly polynomial) {
        for (auto &coefficient : polynomial) coefficient *= value;
        return polynomial;
    }

    friend Poly operator*(Poly polynomial, Value value) { return value * polynomial; }

    friend Poly operator/(Poly polynomial, Value value) {
        for (auto &coefficient : polynomial) coefficient /= value;
        return polynomial;
    }

    Poly &operator+=(const Poly &rhs) { return *this = *this + rhs; }
    Poly &operator-=(const Poly &rhs) { return *this = *this - rhs; }
    Poly &operator*=(const Poly &rhs) { return *this = *this * rhs; }
    Poly &operator*=(Value rhs) { return *this = *this * rhs; }
    Poly &operator/=(Value rhs) { return *this = *this / rhs; }

    Poly deriv() const {
        if (this->empty()) return {};
        Poly result(this->size() - 1);
        for (int i = 0; i + 1 < int(this->size()); i++) result[i] = (i + 1) * (*this)[i + 1];
        return result;
    }

    Poly integr() const {
        Poly result(this->size() + 1);
        for (int i = 0; i < int(this->size()); i++) result[i + 1] = (*this)[i] / (i + 1);
        return result;
    }

    Poly inv(int m) const {
        Poly result{this->empty() ? Value(0) : (*this)[0].inv()};
        int k = 1;
        while (k < m) {
            k <<= 1;
            result = (result * (Poly{2} - trunc(k) * result)).trunc(k);
        }
        return result.trunc(m);
    }

    Poly log(int m) const { return (deriv() * inv(m)).integr().trunc(m); }

    Poly exp(int m) const {
        Poly result{1};
        int k = 1;
        while (k < m) {
            k <<= 1;
            result = (result * (Poly{1} - result.log(k) + trunc(k))).trunc(k);
        }
        return result.trunc(m);
    }

    Poly pow(int k, int m) const {
        int first = 0;
        while (first < int(this->size()) && (*this)[first] == Value(0)) first++;
        if (first == int(this->size()) || i64(first) * k >= m) return Poly(m);
        Value value = (*this)[first];
        Poly normalized = shift(-first) * value.inv();
        return (normalized.log(m - first * k) * k).exp(m - first * k).shift(first * k) * power(value, k);
    }

    Poly sqrt(int m) const {
        assert(!this->empty() && (*this)[0] == Value(1));
        Poly result{1};
        int k = 1;
        Value inverseTwo = Value(2).inv();
        while (k < m) {
            k <<= 1;
            result = (result + (trunc(k) * result.inv(k)).trunc(k)) * inverseTwo;
        }
        return result.trunc(m);
    }

    Poly mulT(Poly b) const {
        if (b.empty()) return {};
        int n = int(b.size());
        std::reverse(b.begin(), b.end());
        return (*this * b).shift(-(n - 1));
    }

    std::vector<Value> eval(std::vector<Value> points) const {
        if (this->empty()) return std::vector<Value>(points.size());
        int n = int(std::max(points.size(), this->size()));
        std::vector<Poly> product(4 * n);
        std::vector<Value> answer(points.size());
        points.resize(n);
        auto build = [&] (auto &&self, int p, int l, int r) -> void {
            if (r - l == 1) {
                product[p] = Poly{1, -points[l]};
                return;
            }
            int mid = (l + r) >> 1;
            self(self, p << 1, l, mid);
            self(self, p << 1 | 1, mid, r);
            product[p] = product[p << 1] * product[p << 1 | 1];
        };
        build(build, 1, 0, n);
        auto work = [&] (auto &&self, int p, int l, int r, const Poly &value) -> void {
            if (r - l == 1) {
                if (l < int(answer.size())) answer[l] = value[0];
                return;
            }
            int mid = (l + r) >> 1;
            self(self, p << 1, l, mid, value.mulT(product[p << 1 | 1]).trunc(mid - l));
            self(self, p << 1 | 1, mid, r, value.mulT(product[p << 1]).trunc(r - mid));
        };
        work(work, 1, 0, n, mulT(product[1].inv(n)));
        return answer;
    }
};
