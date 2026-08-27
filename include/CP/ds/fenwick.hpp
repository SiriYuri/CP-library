#pragma once

#include <vector>

template <class T>
struct Fenwick {
    int n;
    std::vector<T> tree;

    explicit Fenwick(int _n) : n(_n - 1), tree(_n) {}

    void add(int x, T w) { for (int i = x; i <= n; i += i & -i) tree[i] += w; }

    T prefixSum(int x) const {
        T sum {};
        for (; x; x -= x & -x) sum += tree[x];
        return sum;
    }

    T rangeSum(int l, int r) const { return prefixSum(r) - prefixSum(l - 1); }
};
