#pragma once

#include <algorithm>
#include <utility>
#include <vector>

struct BlockCutTree {
    int n, count, current, edgeCount;
    std::vector<std::vector<std::pair<int, int>>> adjacency;
    std::vector<int> dfn, low, stack;
    std::vector<std::pair<int, int>> edges;

    explicit BlockCutTree(int _n) : n(_n - 1), count(0), current(1), edgeCount(0), adjacency(_n), dfn(_n, -1), low(_n) {}

    void addEdge(int u, int v) {
        adjacency[u].emplace_back(v, edgeCount);
        adjacency[v].emplace_back(u, edgeCount);
        edgeCount++;
    }

    void dfs(int u, int parentEdge) {
        stack.push_back(u);
        dfn[u] = low[u] = current++;
        for (auto [v, edgeId] : adjacency[u]) {
            if (edgeId == parentEdge) continue;
            if (dfn[v] == -1) {
                dfs(v, edgeId);
                low[u] = std::min(low[u], low[v]);
                if (low[v] >= dfn[u]) {
                    count++;
                    int currentVertex;
                    do {
                        currentVertex = stack.back();
                        stack.pop_back();
                        edges.emplace_back(n + count, currentVertex);
                    } while (currentVertex != v);
                    edges.emplace_back(u, n + count);
                }
            } else {
                low[u] = std::min(low[u], dfn[v]);
            }
        }
    }

    std::pair<int, std::vector<std::pair<int, int>>> work() {
        for (int i = 1; i <= n; i++) {
            if (dfn[i] == -1) {
                stack.clear();
                dfs(i, -1);
            }
        }
        return {count, edges};
    }
};
