#pragma once

#include "CP/types.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <queue>
#include <random>
#include <utility>
#include <vector>

struct HopcroftKarp {
    int n, m;
    std::vector<std::array<int, 2>> edges;
    std::vector<int> matchl, matchr;

    HopcroftKarp(int _n, int _m) : n(_n - 1), m(_m - 1), matchl(n, -1), matchr(m, -1) {}

    void addEdge(int u, int v) { edges.push_back({u - 1, v - 1}); }

    int work() {
        std::vector<int> adjacency(edges.size());
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::shuffle(edges.begin(), edges.end(), rng);
        std::vector<int> head(n + 1);
        for (auto [u, v] : edges) head[u]++;
        for (int i = 1; i <= n; i++) head[i] += head[i - 1];
        for (auto [u, v] : edges) adjacency[--head[u]] = v;
        int matching = 0;
        std::vector<int> root, parent, queue(n);
        while (true) {
            root.assign(n, -1);
            parent.assign(n, -1);
            int tail = 0;
            for (int i = 0; i < n; i++) {
                if (matchl[i] == -1) {
                    queue[tail++] = i;
                    root[i] = parent[i] = i;
                }
            }
            bool found = false;
            for (int i = 0; i < tail; i++) {
                int u = queue[i];
                if (matchl[root[u]] != -1) continue;
                for (int j = head[u]; j < head[u + 1]; j++) {
                    int v = adjacency[j];
                    if (matchr[v] == -1) {
                        while (v != -1) {
                            matchr[v] = u;
                            std::swap(matchl[u], v);
                            u = parent[u];
                        }
                        found = true;
                        matching++;
                        break;
                    }
                    if (parent[matchr[v]] == -1) {
                        int next = matchr[v];
                        queue[tail++] = next;
                        parent[next] = u;
                        root[next] = root[u];
                    }
                }
            }
            if (!found) break;
        }
        return matching;
    }

    std::vector<std::array<int, 2>> get() const {
        std::vector<std::array<int, 2>> result;
        for (int i = 0; i < n; i++) {
            if (matchl[i] != -1) result.push_back({i + 1, matchl[i] + 1});
        }
        return result;
    }
};

template <typename T>
struct KM {
    int n;
    std::vector<int> ansl, ansr, pre;
    std::vector<T> lx, ly;
    std::vector<std::vector<T>> weight;

    explicit KM(int _n) : n(_n - 1), ansl(_n, -1), ansr(_n, -1), pre(_n, -1), lx(_n, -inf<T>), ly(_n), weight(_n, std::vector<T>(_n, -inf<T>)) {}

    void addEdge(int x, int y, T value) { weight[x][y] = std::max(weight[x][y], value); }

    void bfs(int start) {
        std::vector<bool> visl(n + 1), visr(n + 1);
        std::vector<T> slack(n + 1, inf<T>);
        std::queue<int> queue;
        auto check = [&] (int x) -> bool {
            visr[x] = true;
            if (ansr[x] != -1) {
                queue.push(ansr[x]);
                visl[ansr[x]] = true;
                return false;
            }
            while (x != -1) {
                ansr[x] = pre[x];
                std::swap(x, ansl[pre[x]]);
            }
            return true;
        };
        queue.push(start);
        visl[start] = true;
        while (true) {
            while (!queue.empty()) {
                int x = queue.front();
                queue.pop();
                for (int y = 1; y <= n; y++) {
                    if (visr[y]) continue;
                    T delta = lx[x] + ly[y] - weight[x][y];
                    if (delta < slack[y]) {
                        pre[y] = x;
                        slack[y] = delta;
                        if (slack[y] == 0 && check(y)) return;
                    }
                }
            }
            T value = inf<T>;
            for (int i = 1; i <= n; i++) {
                if (!visr[i]) value = std::min(value, slack[i]);
            }
            for (int i = 1; i <= n; i++) {
                if (visl[i]) lx[i] -= value;
                if (visr[i]) ly[i] += value;
                else slack[i] -= value;
            }
            for (int i = 1; i <= n; i++) {
                if (!visr[i] && slack[i] == 0 && check(i)) return;
            }
        }
    }

    T work() {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) lx[i] = std::max(lx[i], weight[i][j]);
        }
        for (int i = 1; i <= n; i++) bfs(i);
        T result = 0;
        for (int i = 1; i <= n; i++) result += weight[ansr[i]][i];
        return result;
    }

    std::vector<std::pair<int, int>> get() const {
        std::vector<std::pair<int, int>> result;
        for (int i = 1; i <= n; i++) {
            if (ansr[i] != -1 && weight[ansr[i]][i] != -inf<T>) result.emplace_back(ansr[i], i);
        }
        return result;
    }
};
