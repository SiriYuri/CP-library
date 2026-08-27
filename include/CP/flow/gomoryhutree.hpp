#pragma once

#include "CP/flow/maxflow.hpp"

#include <algorithm>
#include <bit>
#include <limits>
#include <numeric>
#include <vector>

template <class T>
struct GomoryHuTree {
    struct Edge {
        int to;
        T w;
    };

    int n;
    MaxFlow<T> mf;
    std::vector<T> icap;
    std::vector<std::vector<Edge>> tree;
    int log;
    std::vector<std::vector<int>> up;
    std::vector<std::vector<T>> minw;
    std::vector<int> dep;

    explicit GomoryHuTree(int _n) : n(_n - 1), mf(_n), tree(_n) { }

    void addEdge(int u, int v, T wgt) {
        mf.addEdge(u, v, wgt);
        mf.addEdge(v, u, wgt);
    }

    void solve(std::vector<int> &nods) {
        if (nods.size() <= 1) { return ; }
        int s = nods[0], t = nods[1];
        for (int i = 0; i < int(mf.e.size()); ++i) mf.e[i].cap = icap[i];
        T cut = mf.flow(s, t);
        tree[s].push_back({t, cut});
        tree[t].push_back({s, cut});
        std::vector<int> left, rgt;
        for (int u : nods) {
            if (mf.h[u] != -1) { left.push_back(u); }
            else rgt.push_back(u);
        }
        solve(left);
        solve(rgt);
    }

    void dfs(int u, int par, T wgt) {
        dep[u] = dep[par] + 1;
        up[u][0] = par;
        minw[u][0] = wgt;
        for (int i = 1; i < log; ++i) {
            up[u][i] = up[up[u][i - 1]][i - 1];
            minw[u][i] = std::min(minw[u][i - 1], minw[up[u][i - 1]][i - 1]);
        }
        for (auto [v, w] : tree[u]) {
            if (v != par) { dfs(v, u, w); }
        }
    }

    void buildLca() {
        log = int(std::bit_width(unsigned(n)));
        up.assign(n + 1, std::vector<int>(log));
        minw.assign(n + 1, std::vector<T>(log, std::numeric_limits<T>::max()));
        dep.assign(n + 1, 0);
        dfs(1, 0, std::numeric_limits<T>::max());
    }

    void build() {
        tree.assign(n + 1, { });
        icap.resize(mf.e.size());
        for (int i = 0; i < int(mf.e.size()); ++i) icap[i] = mf.e[i].cap;
        std::vector<int> nods(n);
        std::iota(nods.begin(), nods.end(), 1);
        solve(nods);
        buildLca();
    }

    T query(int u, int v) const {
        if (dep[u] < dep[v]) { std::swap(u, v); }
        T ans = std::numeric_limits<T>::max();
        for (int i = log - 1; i >= 0; --i) {
            if (dep[u] - (1 << i) >= dep[v]) {
                ans = std::min(ans, minw[u][i]);
                u = up[u][i];
            }
        }
        if (u == v) { return ans; }
        for (int i = log - 1; i >= 0; --i) {
            if (up[u][i] != up[v][i]) {
                ans = std::min({ans, minw[u][i], minw[v][i]});
                u = up[u][i];
                v = up[v][i];
            }
        }
        return std::min({ans, minw[u][0], minw[v][0]});
    }
};
