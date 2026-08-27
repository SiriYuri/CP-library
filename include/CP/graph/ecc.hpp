#pragma once

#include <algorithm>
#include <array>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

struct ECC {
    int n, tot, cnt, ecnt;
    std::vector<int> dfn, low, id, stk;
    std::vector<std::vector<std::pair<int, int>>> edge;
    std::vector<std::vector<int>> adj;
    std::set<std::array<int, 2>> bridge;

    explicit ECC(int _n) : n(_n - 1), tot(0), cnt(0), ecnt(0), dfn(_n, -1), low(_n), id(_n, -1), edge(_n) {}

    void add(int u, int v) {
        edge[u].emplace_back(v, ecnt);
        edge[v].emplace_back(u, ecnt);
        ecnt++;
    }

    void tarjan(int u, int pedge) {
        dfn[u] = low[u] = ++tot;
        stk.emplace_back(u);
        for (auto [v, eid] : edge[u]) {
            if (eid == pedge) { continue; }
            if (dfn[v] == -1) {
                tarjan(v, eid);
                low[u] = std::min(low[u], low[v]);
                if (low[v] > dfn[u]) { bridge.insert({u, v}); }
            } else if (id[v] == -1 && dfn[v] < dfn[u]) {
                low[u] = std::min(low[u], dfn[v]);
            }
        }
        if (dfn[u] == low[u]) {
            int v;
            cnt++;
            do {
                v = stk.back();
                stk.pop_back();
                id[v] = cnt;
            } while (v != u);
        }
    }

    auto work() {
        for (int i = 1; i <= n; i++) {
            if (dfn[i] == -1) { tarjan(i, -1); }
        }
        std::vector<int> size(cnt + 1);
        adj.assign(cnt + 1, {});
        for (int u = 1; u <= n; u++) {
            size[id[u]]++;
            for (auto [v, eid] : edge[u]) {
                int x = id[u], y = id[v];
                if (x != y) { adj[x].emplace_back(y); }
            }
        }
        return std::make_tuple(cnt, adj, id, size);
    }
};
