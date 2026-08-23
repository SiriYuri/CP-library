#pragma once

#include <numeric>
#include <vector>

struct DSU {
    int n;
    std::vector<int> parent;
    std::vector<int> _size;
    
    explicit DSU(int _n) : n(_n - 1), parent(_n), _size(_n, 1) {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) { return x == parent[x] ? x : parent[x] = find(parent[x]); }

    bool same(int x, int y) { return find(x) == find(y); }

    void merge(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return ;
        }
        parent[x] = y;
        _size[y] += _size[x];
    }

    int size(int x) { return _size[find(x)]; }
};
