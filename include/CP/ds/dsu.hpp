#pragma once

#include <numeric>
#include <vector>

struct DSU {
    int n;
    std::vector<int> par;
    std::vector<int> siz;

    explicit DSU(int _n) : n(_n - 1), par(_n), siz(_n, 1) {
        std::iota(par.begin(), par.end(), 0);
    }

    int find(int x) { return x == par[x] ? x : par[x] = find(par[x]); }

    bool same(int x, int y) { return find(x) == find(y); }

    void merge(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) { return ; }
        par[x] = y;
        siz[y] += siz[x];
    }

    int size(int x) { return siz[find(x)]; }
};
