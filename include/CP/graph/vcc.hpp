#pragma once

#include <algorithm>
#include <vector>

struct VCC {
    int n, tot, cnt;
    std::vector<int> isCut, dfn, low, stk;
    std::vector<std::vector<int>> edge, comp;

    explicit VCC(int _n) : n(_n - 1), tot(0), cnt(0), isCut(_n), dfn(_n), low(_n), edge(_n), comp(_n) {}

    void add(int u, int v) {
        edge[u].emplace_back(v);
        edge[v].emplace_back(u);
    }

    void tarjan(int u, int root) {
        low[u] = dfn[u] = ++tot;
        stk.emplace_back(u);
        if (u == root && edge[u].empty()) {
            cnt++;
            comp[cnt].emplace_back(u);
            stk.pop_back();
            return;
        }
        int son = 0;
        for (int v : edge[u]) {
            if (!dfn[v]) {
                tarjan(v, root);
                low[u] = std::min(low[u], low[v]);
                if (dfn[u] <= low[v]) {
                    son++;
                    if (u != root || son > 1) { isCut[u] = 1; }
                    int cur;
                    cnt++;
                    do {
                        cur = stk.back();
                        stk.pop_back();
                        comp[cnt].emplace_back(cur);
                    } while (cur != v);
                    comp[cnt].emplace_back(u);
                }
            } else {
                low[u] = std::min(low[u], dfn[v]);
            }
        }
    }

    void work() {
        for (int i = 1; i <= n; i++) {
            if (!dfn[i]) { tarjan(i, i); }
        }
    }
};
