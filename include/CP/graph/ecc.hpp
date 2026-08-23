#pragma once

#include <algorithm>
#include <array>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

struct ECC {
    int n, total, count, edgeCount;
    std::vector<int> dfn, low, id, stack;
    std::vector<std::vector<std::pair<int, int>>> edge;
    std::vector<std::vector<int>> adjacency;
    std::set<std::array<int, 2>> bridge;

    explicit ECC(int _n) : n(_n - 1), total(0), count(0), edgeCount(0), dfn(_n, -1), low(_n), id(_n, -1), edge(_n) {}

    void add(int u, int v) {
        edge[u].emplace_back(v, edgeCount);
        edge[v].emplace_back(u, edgeCount);
        edgeCount++;
    }

    void Tarjan(int u, int parentEdge) {
        dfn[u] = low[u] = ++total;
        stack.emplace_back(u);
        for (auto [v, edgeId] : edge[u]) {
            if (edgeId == parentEdge) continue;
            if (dfn[v] == -1) {
                Tarjan(v, edgeId);
                low[u] = std::min(low[u], low[v]);
                if (low[v] > dfn[u]) bridge.insert({u, v});
            } else if (id[v] == -1 && dfn[v] < dfn[u]) {
                low[u] = std::min(low[u], dfn[v]);
            }
        }
        if (dfn[u] == low[u]) {
            int v;
            count++;
            do {
                v = stack.back();
                stack.pop_back();
                id[v] = count;
            } while (v != u);
        }
    }

    auto work() {
        for (int i = 1; i <= n; i++) {
            if (dfn[i] == -1) Tarjan(i, -1);
        }
        std::vector<int> size(count + 1);
        adjacency.assign(count + 1, {});
        for (int u = 1; u <= n; u++) {
            size[id[u]]++;
            for (auto [v, edgeId] : edge[u]) {
                int x = id[u], y = id[v];
                if (x != y) adjacency[x].emplace_back(y);
            }
        }
        return std::make_tuple(count, adjacency, id, size);
    }
};
