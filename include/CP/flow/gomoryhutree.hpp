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
    std::vector<T> initialCap;
    std::vector<std::vector<Edge>> tree;
    int log;
    std::vector<std::vector<int>> up;
    std::vector<std::vector<T>> minWeight;
    std::vector<int> depth;

    explicit GomoryHuTree(int _n) : n(_n - 1), mf(_n), tree(_n) { }

    void addEdge(int u, int v, T weight) {
        mf.addEdge(u, v, weight);
        mf.addEdge(v, u, weight);
    }

    void solve(std::vector<int> &nodes) {
        if (nodes.size() <= 1) return ;
        int s = nodes[0], t = nodes[1];
        for (int i = 0; i < int(mf.e.size()); ++i) mf.e[i].cap = initialCap[i];
        T cut = mf.flow(s, t);
        tree[s].push_back({t, cut});
        tree[t].push_back({s, cut});
        std::vector<int> left, right;
        for (int u : nodes) {
            if (mf.h[u] != -1) left.push_back(u);
            else right.push_back(u);
        }
        solve(left);
        solve(right);
    }

    void dfs(int u, int parent, T weight) {
        depth[u] = depth[parent] + 1;
        up[u][0] = parent;
        minWeight[u][0] = weight;
        for (int i = 1; i < log; ++i) {
            up[u][i] = up[up[u][i - 1]][i - 1];
            minWeight[u][i] = std::min(minWeight[u][i - 1], minWeight[up[u][i - 1]][i - 1]);
        }
        for (auto [v, w] : tree[u]) {
            if (v != parent) dfs(v, u, w);
        }
    }

    void buildLca() {
        log = int(std::bit_width(unsigned(n)));
        up.assign(n + 1, std::vector<int>(log));
        minWeight.assign(n + 1, std::vector<T>(log, std::numeric_limits<T>::max()));
        depth.assign(n + 1, 0);
        dfs(1, 0, std::numeric_limits<T>::max());
    }

    void build() {
        tree.assign(n + 1, { });
        initialCap.resize(mf.e.size());
        for (int i = 0; i < int(mf.e.size()); ++i) initialCap[i] = mf.e[i].cap;
        std::vector<int> nodes(n);
        std::iota(nodes.begin(), nodes.end(), 1);
        solve(nodes);
        buildLca();
    }

    T query(int u, int v) const {
        if (depth[u] < depth[v]) std::swap(u, v);
        T ans = std::numeric_limits<T>::max();
        for (int i = log - 1; i >= 0; --i) {
            if (depth[u] - (1 << i) >= depth[v]) {
                ans = std::min(ans, minWeight[u][i]);
                u = up[u][i];
            }
        }
        if (u == v) return ans;
        for (int i = log - 1; i >= 0; --i) {
            if (up[u][i] != up[v][i]) {
                ans = std::min({ans, minWeight[u][i], minWeight[v][i]});
                u = up[u][i];
                v = up[v][i];
            }
        }
        return std::min({ans, minWeight[u][0], minWeight[v][0]});
    }
};
