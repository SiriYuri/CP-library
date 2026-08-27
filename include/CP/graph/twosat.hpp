#pragma once

#include <algorithm>
#include <vector>

struct TwoSat {
    int n;
    std::vector<std::vector<int>> edge;
    std::vector<bool> ans;

    explicit TwoSat(int _n) : n(_n - 1), edge(2 * n + 2), ans(_n) {}

    void add(int u, bool f, int v, bool g) {
        edge[2 * u + !f].emplace_back(2 * v + g);
        edge[2 * v + !g].emplace_back(2 * u + f);
    }

    bool work() {
        std::vector<int> id(2 * n + 2, -1), dfn(2 * n + 2, -1), low(2 * n + 2, -1), stk;
        int now = 0, cnt = 0;
        auto tarjan = [&] (auto &&self, int u) -> void {
            stk.emplace_back(u);
            dfn[u] = low[u] = now++;
            for (int v : edge[u]) {
                if (dfn[v] == -1) {
                    self(self, v);
                    low[u] = std::min(low[u], low[v]);
                } else if (id[v] == -1) {
                    low[u] = std::min(low[u], dfn[v]);
                }
            }
            if (dfn[u] == low[u]) {
                int v;
                do {
                    v = stk.back();
                    stk.pop_back();
                    id[v] = cnt;
                } while (v != u);
                cnt++;
            }
        };

        for (int i = 2; i <= 2 * n + 1; i++) {
            if (dfn[i] == -1) { tarjan(tarjan, i); }
        }
        for (int i = 1; i <= n; i++) {
            if (id[2 * i] == id[2 * i + 1]) { return false; }
            ans[i] = id[2 * i] > id[2 * i + 1];
        }
        return true;
    }
};
