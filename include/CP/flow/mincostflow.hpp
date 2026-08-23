#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

template <class T>
struct MinCostFlow {
    struct _Edge {
        int to;
        T cap;
        T cost;

        _Edge(int _to, T _cap, T _cost) : to(_to), cap(_cap), cost(_cost) { }
    };

    int n;
    std::vector<_Edge> e;
    std::vector<std::vector<int>> g;
    std::vector<T> h;
    std::vector<T> dis;
    std::vector<int> pre;

    MinCostFlow() = default;

    explicit MinCostFlow(int _n) { init(_n); }

    void init(int _n) {
        n = _n - 1;
        e.clear();
        g.assign(_n, { });
        h.resize(_n);
        dis.resize(_n);
        pre.resize(_n);
    }

    void addEdge(int u, int v, T cap, T cost) {
        g[u].push_back(int(e.size()));
        e.emplace_back(v, cap, cost);
        g[v].push_back(int(e.size()));
        e.emplace_back(u, 0, -cost);
    }

    bool dijkstra(int s, int t) {
        dis.assign(n + 1, std::numeric_limits<T>::max());
        pre.assign(n + 1, -1);
        std::priority_queue<std::pair<T, int>, std::vector<std::pair<T, int>>, std::greater<std::pair<T, int>>> que;
        dis[s] = 0;
        que.emplace(0, s);
        while (!que.empty()) {
            auto [d, u] = que.top();
            que.pop();
            if (dis[u] != d) continue;
            for (int i : g[u]) {
                int v = e[i].to;
                T cap = e[i].cap;
                T cost = e[i].cost;
                if (cap > 0 && dis[v] > d + h[u] - h[v] + cost) {
                    dis[v] = d + h[u] - h[v] + cost;
                    pre[v] = i;
                    que.emplace(dis[v], v);
                }
            }
        }
        return dis[t] != std::numeric_limits<T>::max();
    }

    std::pair<T, T> flow(int s, int t) {
        T maxFlow = 0;
        T minCost = 0;
        h.assign(n + 1, 0);
        while (dijkstra(s, t)) {
            for (int i = 1; i <= n; ++i) {
                if (dis[i] != std::numeric_limits<T>::max()) h[i] += dis[i];
            }
            T aug = std::numeric_limits<T>::max();
            for (int i = t; i != s; i = e[pre[i] ^ 1].to) aug = std::min(aug, e[pre[i]].cap);
            for (int i = t; i != s; i = e[pre[i] ^ 1].to) {
                e[pre[i]].cap -= aug;
                e[pre[i] ^ 1].cap += aug;
            }
            maxFlow += aug;
            minCost += aug * h[t];
        }
        return {maxFlow, minCost};
    }

    struct Edge {
        int from;
        int to;
        T cap;
        T cost;
        T flow;
    };

    std::vector<Edge> edges() const {
        std::vector<Edge> ans;
        for (int i = 0; i < int(e.size()); i += 2) {
            ans.push_back({e[i + 1].to, e[i].to, e[i].cap + e[i + 1].cap, e[i].cost, e[i + 1].cap});
        }
        return ans;
    }
};
