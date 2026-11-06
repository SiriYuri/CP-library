#pragma once

#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

template <class T>
struct MaxFlow {
    struct _Edge {
        int to;
        T cap;

        _Edge(int _to, T _cap) : to(_to), cap(_cap) { }
    };

    int n;
    std::vector<_Edge> e;
    std::vector<std::vector<int>> g;
    std::vector<int> cur;
    std::vector<int> h;

    MaxFlow() = default;

    explicit MaxFlow(int _n) { init(_n); }

    void init(int _n) {
        n = _n - 1;
        e.clear();
        g.assign(_n, { });
        cur.resize(_n);
        h.resize(_n);
    }

    void addEdge(int u, int v, T cap) {
        g[u].push_back(int(e.size()));
        e.emplace_back(v, cap);
        g[v].push_back(int(e.size()));
        e.emplace_back(u, 0);
    }

    bool bfs(int s, int t) {
        h.assign(n + 1, -1);
        std::queue<int> que;
        h[s] = 0;
        que.push(s);
        while (!que.empty()) {
            int u = que.front();
            que.pop();
            for (int i : g[u]) {
                auto [v, cap] = e[i];
                if (cap > 0 && h[v] == -1) {
                    h[v] = h[u] + 1;
                    if (v == t) { return true; }
                    que.push(v);
                }
            }
        }
        return false;
    }

    T dfs(int u, int t, T flow) {
        if (u == t) { return flow; }
        T rest = flow;
        for (int &i = cur[u]; i < int(g[u].size()); ++i) {
            int j = g[u][i];
            auto [v, cap] = e[j];
            if (cap > 0 && h[v] == h[u] + 1) {
                T aug = dfs(v, t, std::min(rest, cap));
                if (aug == 0) {
                    h[v] = -1;
                } else {
                    e[j].cap -= aug;
                    e[j ^ 1].cap += aug;
                    rest -= aug;
                    if (rest == 0) { return flow; }
                }
            }
        }
        return flow - rest;
    }

    T flow(int s, int t) {
        T ans = 0;
        while (bfs(s, t)) {
            cur.assign(n + 1, 0);
            ans += dfs(s, t, std::numeric_limits<T>::max());
        }
        return ans;
    }

    std::vector<bool> minCut() const {
        std::vector<bool> cut(n + 1);
        for (int i = 1; i <= n; ++i) cut[i] = h[i] != -1;
        return cut;
    }

    struct Edge {
        int from;
        int to;
        T cap;
        T flow;
    };

    std::vector<Edge> eds() const {
        std::vector<Edge> ans;
        for (int i = 0; i < int(e.size()); i += 2) {
            ans.push_back({e[i + 1].to, e[i].to, e[i].cap + e[i + 1].cap, e[i + 1].cap});
        }
        return ans;
    }
};
