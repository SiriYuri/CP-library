#pragma once

#include <algorithm>
#include <utility>
#include <vector>

struct BlockCutTree {
    int n, cnt, cur, ecnt;
    std::vector<std::vector<std::pair<int, int>>> adj;
    std::vector<int> dfn, low, stk;
    std::vector<std::pair<int, int>> eds;

    explicit BlockCutTree(int _n) : n(_n - 1), cnt(0), cur(1), ecnt(0), adj(_n), dfn(_n, -1), low(_n) {}

    void addEdge(int u, int v) {
        adj[u].emplace_back(v, ecnt);
        adj[v].emplace_back(u, ecnt);
        ecnt++;
    }

    void dfs(int u, int pedge) {
        stk.push_back(u);
        dfn[u] = low[u] = cur++;
        for (auto [v, eid] : adj[u]) {
            if (eid == pedge) { continue; }
            if (dfn[v] == -1) {
                dfs(v, eid);
                low[u] = std::min(low[u], low[v]);
                if (low[v] >= dfn[u]) {
                    cnt++;
                    int curv;
                    do {
                        curv = stk.back();
                        stk.pop_back();
                        eds.emplace_back(n + cnt, curv);
                    } while (curv != v);
                    eds.emplace_back(u, n + cnt);
                }
            } else {
                low[u] = std::min(low[u], dfn[v]);
            }
        }
    }

    std::pair<int, std::vector<std::pair<int, int>>> work() {
        for (int i = 1; i <= n; i++) {
            if (dfn[i] == -1) {
                stk.clear();
                dfs(i, -1);
            }
        }
        return {cnt, eds};
    }
};
