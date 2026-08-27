#pragma once

#include "CP/types.hpp"

#include <utility>
#include <vector>

namespace SegtGraph {
struct Graph {
    int n, shift;
    std::vector<int> leaf;
    std::vector<std::vector<std::pair<int, i64>>> edge;

private:
    void build(int p, int l, int r) {
        if (l == r) {
            leaf[l] = p;
            return;
        }
        int mid = (l + r) >> 1;
        edge[p].emplace_back(p << 1, 0);
        edge[p].emplace_back(p << 1 | 1, 0);
        edge[(p << 1) + shift].emplace_back(p + shift, 0);
        edge[(p << 1 | 1) + shift].emplace_back(p + shift, 0);
        build(p << 1, l, mid);
        build(p << 1 | 1, mid + 1, r);
    }

    void link(bool rev, int p, int l, int r, int x, int y, int pos, i64 wgt) {
        if (x > r || y < l) { return; }
        if (x <= l && r <= y) {
            if (!rev) { edge[pos].emplace_back(p, wgt); }
            else edge[p + shift].emplace_back(pos, wgt);
            return;
        }
        int mid = (l + r) >> 1;
        link(rev, p << 1, l, mid, x, y, pos, wgt);
        link(rev, p << 1 | 1, mid + 1, r, x, y, pos, wgt);
    }

public:
    explicit Graph(int _n) : n(_n - 1), shift(n << 2), leaf(_n), edge((n << 3) + 1) {
        build(1, 1, n);
        for (int i = 1; i <= n; i++) {
            int u = leaf[i], v = u + shift;
            edge[u].emplace_back(v, 0);
            edge[v].emplace_back(u, 0);
        }
    }

    void addPointToPoint(int u, int v, i64 wgt = 1) { edge[leaf[u]].emplace_back(leaf[v], wgt); }

    void addPointToSeg(int u, int l, int r, i64 wgt = 1) { link(false, 1, 1, n, l, r, leaf[u], wgt); }

    void addSegToPoint(int l, int r, int v, i64 wgt = 1) { link(true, 1, 1, n, l, r, leaf[v], wgt); }

    int get(int p) const { return leaf[p]; }
};
}
