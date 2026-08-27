#pragma once

#include "CP/types.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>

struct StoerWagner {
    int n;
    std::vector<std::vector<i64>> g;
    std::vector<i64> w;
    std::vector<int> id;
    std::vector<bool> vis;

    explicit StoerWagner(int _n) : n(_n - 1), g(_n, std::vector<i64>(_n)), w(_n), id(_n), vis(_n) {
        std::iota(id.begin(), id.end(), 0);
    }

    void addEdge(int u, int v, i64 wgt) {
        g[u][v] += wgt;
        g[v][u] += wgt;
    }

    i64 minCut() {
        if (n < 2) { return 0; }
        i64 ans = std::numeric_limits<i64>::max();
        for (int size = n; size > 1; --size) {
            std::fill(w.begin(), w.end(), 0);
            std::fill(vis.begin(), vis.end(), false);
            int prev = 0;
            for (int i = 1; i <= size; ++i) {
                int next = -1;
                for (int j = 1; j <= size; ++j) {
                    if (!vis[id[j]] && (next == -1 || w[id[j]] > w[id[next]])) { next = j; }
                }
                vis[id[next]] = true;
                if (i == size) {
                    ans = std::min(ans, w[id[next]]);
                    for (int j = 1; j <= size; ++j) {
                        g[id[prev]][id[j]] += g[id[next]][id[j]];
                        g[id[j]][id[prev]] = g[id[prev]][id[j]];
                    }
                    id[next] = id[size];
                } else {
                    prev = next;
                    for (int j = 1; j <= size; ++j) {
                        if (!vis[id[j]]) { w[id[j]] += g[id[next]][id[j]]; }
                    }
                }
            }
        }
        return ans;
    }
};
