#pragma once

#include <algorithm>
#include <tuple>
#include <vector>

struct SCC {
    int n, total, count, current;
    std::vector<int> dfn, low, belong, stack;
    std::vector<std::vector<int>> adjacency;

    explicit SCC(int _n) : n(_n - 1), total(0), count(0), current(-1), dfn(_n), low(_n), belong(_n), adjacency(_n) {}

    void add(int u, int v) { adjacency[u].emplace_back(v); }

    void Tarjan(int u) {
        dfn[u] = low[u] = ++total;
        stack.emplace_back(u);
        for (int v : adjacency[u]) {
            if (!dfn[v]) {
                Tarjan(v);
                low[u] = std::min(low[u], low[v]);
            } else if (!belong[v]) {
                low[u] = std::min(low[u], dfn[v]);
            }
        }
        if (dfn[u] == low[u]) {
            count++;
            do {
                current = stack.back();
                stack.pop_back();
                belong[current] = count;
            } while (current != u);
        }
    }

    auto work() {
        for (int i = 1; i <= n; i++) {
            if (!dfn[i]) Tarjan(i);
        }
        std::vector<int> size(count + 1);
        std::vector<std::vector<int>> edge(count + 1);
        for (int u = 1; u <= n; u++) {
            size[belong[u]]++;
            for (int v : adjacency[u]) {
                int x = belong[u], y = belong[v];
                if (x != y) edge[x].emplace_back(y);
            }
        }
        return std::make_tuple(count, edge, belong, size);
    }
};
