#pragma once

#include <algorithm>
#include <tuple>
#include <vector>

struct SCC {
    int n, tot, cnt, cur;
    std::vector<int> dfn, low, bel, stk;
    std::vector<std::vector<int>> adj;

    explicit SCC(int _n) : n(_n - 1), tot(0), cnt(0), cur(-1), dfn(_n), low(_n), bel(_n), adj(_n) {}

    void add(int u, int v) { adj[u].emplace_back(v); }

    void tarjan(int u) {
        dfn[u] = low[u] = ++tot;
        stk.emplace_back(u);
        for (int v : adj[u]) {
            if (!dfn[v]) {
                tarjan(v);
                low[u] = std::min(low[u], low[v]);
            } else if (!bel[v]) {
                low[u] = std::min(low[u], dfn[v]);
            }
        }
        if (dfn[u] == low[u]) {
            cnt++;
            do {
                cur = stk.back();
                stk.pop_back();
                bel[cur] = cnt;
            } while (cur != u);
        }
    }

    auto work() {
        for (int i = 1; i <= n; i++) {
            if (!dfn[i]) { tarjan(i); }
        }
        std::vector<int> size(cnt + 1);
        std::vector<std::vector<int>> edge(cnt + 1);
        for (int u = 1; u <= n; u++) {
            size[bel[u]]++;
            for (int v : adj[u]) {
                int x = bel[u], y = bel[v];
                if (x != y) { edge[x].emplace_back(y); }
            }
        }
        return std::make_tuple(cnt, edge, bel, size);
    }
};
