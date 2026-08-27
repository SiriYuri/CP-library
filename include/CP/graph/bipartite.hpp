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
    std::vector<std::array<int, 2>> eds;
    std::vector<int> matl, matr;

    HopcroftKarp(int _n, int _m) : n(_n - 1), m(_m - 1), matl(n, -1), matr(m, -1) {}

    void addEdge(int u, int v) { eds.push_back({u - 1, v - 1}); }

    int work() {
        std::vector<int> adj(eds.size());
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::shuffle(eds.begin(), eds.end(), rng);
        std::vector<int> head(n + 1);
        for (auto [u, v] : eds) head[u]++;
        for (int i = 1; i <= n; i++) head[i] += head[i - 1];
        for (auto [u, v] : eds) adj[--head[u]] = v;
        int mtch = 0;
        std::vector<int> root, par, que(n);
        while (true) {
            root.assign(n, -1);
            par.assign(n, -1);
            int tail = 0;
            for (int i = 0; i < n; i++) {
                if (matl[i] == -1) {
                    que[tail++] = i;
                    root[i] = par[i] = i;
                }
            }
            bool found = false;
            for (int i = 0; i < tail; i++) {
                int u = que[i];
                if (matl[root[u]] != -1) { continue; }
                for (int j = head[u]; j < head[u + 1]; j++) {
                    int v = adj[j];
                    if (matr[v] == -1) {
                        while (v != -1) {
                            matr[v] = u;
                            std::swap(matl[u], v);
                            u = par[u];
                        }
                        found = true;
                        mtch++;
                        break;
                    }
                    if (par[matr[v]] == -1) {
                        int next = matr[v];
                        que[tail++] = next;
                        par[next] = u;
                        root[next] = root[u];
                    }
                }
            }
            if (!found) { break; }
        }
        return mtch;
    }

    std::vector<std::array<int, 2>> get() const {
        std::vector<std::array<int, 2>> res;
        for (int i = 0; i < n; i++) {
            if (matl[i] != -1) { res.push_back({i + 1, matl[i] + 1}); }
        }
        return res;
    }
};

template <typename T>
struct KM {
    int n;
    std::vector<int> ansl, ansr, pre;
    std::vector<T> lx, ly;
    std::vector<std::vector<T>> wgt;

    explicit KM(int _n) : n(_n - 1), ansl(_n, -1), ansr(_n, -1), pre(_n, -1), lx(_n, -inf<T>), ly(_n), wgt(_n, std::vector<T>(_n, -inf<T>)) {}

    void addEdge(int x, int y, T val) { wgt[x][y] = std::max(wgt[x][y], val); }

    void bfs(int start) {
        std::vector<bool> visl(n + 1), visr(n + 1);
        std::vector<T> slk(n + 1, inf<T>);
        std::queue<int> que;
        auto check = [&] (int x) -> bool {
            visr[x] = true;
            if (ansr[x] != -1) {
                que.push(ansr[x]);
                visl[ansr[x]] = true;
                return false;
            }
            while (x != -1) {
                ansr[x] = pre[x];
                std::swap(x, ansl[pre[x]]);
            }
            return true;
        };

        que.push(start);
        visl[start] = true;
        while (true) {
            while (!que.empty()) {
                int x = que.front();
                que.pop();
                for (int y = 1; y <= n; y++) {
                    if (visr[y]) { continue; }
                    T dt = lx[x] + ly[y] - wgt[x][y];
                    if (dt < slk[y]) {
                        pre[y] = x;
                        slk[y] = dt;
                        if (slk[y] == 0 && check(y)) { return; }
                    }
                }
            }
            T val = inf<T>;
            for (int i = 1; i <= n; i++) {
                if (!visr[i]) { val = std::min(val, slk[i]); }
            }
            for (int i = 1; i <= n; i++) {
                if (visl[i]) { lx[i] -= val; }
                if (visr[i]) { ly[i] += val; }
                else slk[i] -= val;
            }
            for (int i = 1; i <= n; i++) {
                if (!visr[i] && slk[i] == 0 && check(i)) { return; }
            }
        }
    }

    T work() {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) lx[i] = std::max(lx[i], wgt[i][j]);
        }
        for (int i = 1; i <= n; i++) bfs(i);
        T res = 0;
        for (int i = 1; i <= n; i++) res += wgt[ansr[i]][i];
        return res;
    }

    std::vector<std::pair<int, int>> get() const {
        std::vector<std::pair<int, int>> res;
        for (int i = 1; i <= n; i++) {
            if (ansr[i] != -1 && wgt[ansr[i]][i] != -inf<T>) { res.emplace_back(ansr[i], i); }
        }
        return res;
    }
};
