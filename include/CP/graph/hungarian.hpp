#pragma once

#include <utility>
#include <vector>

struct Hungarian {
    int n, m, time;
    std::vector<std::vector<int>> adjacency;
    std::vector<int> matchl, matchr, visited;

    Hungarian(int _n, int _m) : n(_n - 1), m(_m - 1), time(0), adjacency(_n), matchl(_n, -1), matchr(_m, -1), visited(_m) {}

    void addEdge(int u, int v) { adjacency[u].emplace_back(v); }

    bool dfs(int u) {
        for (int v : adjacency[u]) {
            if (visited[v] == time) continue;
            visited[v] = time;
            if (matchr[v] == -1 || dfs(matchr[v])) {
                matchr[v] = u;
                matchl[u] = v;
                return true;
            }
        }
        return false;
    }

    int work() {
        int result = 0;
        for (int i = n; i >= 1; i--) {
            time++;
            if (dfs(i)) result++;
        }
        return result;
    }

    std::vector<std::pair<int, int>> edges() const {
        std::vector<std::pair<int, int>> result;
        for (int i = 1; i <= n; i++) {
            if (matchl[i] != -1) result.emplace_back(i, matchl[i]);
        }
        return result;
    }
};
