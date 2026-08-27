#pragma once

#include <utility>
#include <vector>

struct Hungarian {
    int n, m, time;
    std::vector<std::vector<int>> adj;
    std::vector<int> matl, matr, vis;

    Hungarian(int _n, int _m) : n(_n - 1), m(_m - 1), time(0), adj(_n), matl(_n, -1), matr(_m, -1), vis(_m) {}

    void addEdge(int u, int v) { adj[u].emplace_back(v); }

    bool dfs(int u) {
        for (int v : adj[u]) {
            if (vis[v] == time) { continue; }
            vis[v] = time;
            if (matr[v] == -1 || dfs(matr[v])) {
                matr[v] = u;
                matl[u] = v;
                return true;
            }
        }
        return false;
    }

    int work() {
        int res = 0;
        for (int i = n; i >= 1; i--) {
            time++;
            if (dfs(i)) { res++; }
        }
        return res;
    }

    std::vector<std::pair<int, int>> eds() const {
        std::vector<std::pair<int, int>> res;
        for (int i = 1; i <= n; i++) {
            if (matl[i] != -1) { res.emplace_back(i, matl[i]); }
        }
        return res;
    }
};
