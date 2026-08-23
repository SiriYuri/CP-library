#pragma once

#include <algorithm>
#include <vector>

struct VCC {
    int n, total, count;
    std::vector<int> isCut, dfn, low, stack;
    std::vector<std::vector<int>> edge, component;

    explicit VCC(int _n) : n(_n - 1), total(0), count(0), isCut(_n), dfn(_n), low(_n), edge(_n), component(_n) {}

    void add(int u, int v) {
        edge[u].emplace_back(v);
        edge[v].emplace_back(u);
    }

    void Tarjan(int u, int root) {
        low[u] = dfn[u] = ++total;
        stack.emplace_back(u);
        if (u == root && edge[u].empty()) {
            count++;
            component[count].emplace_back(u);
            stack.pop_back();
            return;
        }
        int children = 0;
        for (int v : edge[u]) {
            if (!dfn[v]) {
                Tarjan(v, root);
                low[u] = std::min(low[u], low[v]);
                if (dfn[u] <= low[v]) {
                    children++;
                    if (u != root || children > 1) isCut[u] = 1;
                    int current;
                    count++;
                    do {
                        current = stack.back();
                        stack.pop_back();
                        component[count].emplace_back(current);
                    } while (current != v);
                    component[count].emplace_back(u);
                }
            } else {
                low[u] = std::min(low[u], dfn[v]);
            }
        }
    }

    void work() {
        for (int i = 1; i <= n; i++) {
            if (!dfn[i]) Tarjan(i, i);
        }
    }
};
