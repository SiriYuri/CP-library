#pragma once

#include "CP/types.hpp"

#include <algorithm>
#include <numeric>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

template <typename T = i64>
struct BlossomTree {
    struct Edge {
        int u, v;
        T w;
    };

    int n, m, tms;
    std::vector<std::vector<Edge>> adj;
    std::vector<std::vector<int>> base, bnod;
    std::vector<T> lab;
    std::vector<int> mat, slk, top, par, col, vtim;
    std::queue<int> que;

    explicit BlossomTree(int _n) : n(_n - 1) {
        int size = 2 * n + 2;
        adj.assign(size, std::vector<Edge>(size));
        base.assign(size, std::vector<int>(size));
        bnod.resize(size);
        lab.assign(size, 0);
        mat.assign(size, 0);
        slk.assign(size, 0);
        top.assign(size, 0);
        par.assign(size, 0);
        col.assign(size, 0);
        vtim.assign(size, 0);
    }

    T calcDelta(const Edge &edge) const { return lab[edge.u] + lab[edge.v] - edge.w * 2; }

    void updateSlack(int u, int v) {
        if (!slk[v] || calcDelta(adj[u][v]) < calcDelta(adj[slk[v]][v])) { slk[v] = u; }
    }

    void setSlack(int v) {
        slk[v] = 0;
        for (int u = 1; u <= n; u++) {
            if (adj[u][v].w > 0 && top[u] != v && !col[top[u]]) { updateSlack(u, v); }
        }
    }

    void enqueue(int u) {
        if (u <= n) { que.emplace(u); }
        else for (int v : bnod[u]) enqueue(v);
    }

    void setTop(int u, int val) {
        top[u] = val;
        if (u > n) { for (int v : bnod[u]) setTop(v, val); }
    }

    int getPosition(int u, int v) {
        int pos = int(std::find(bnod[u].begin(), bnod[u].end(), v) - bnod[u].begin());
        if (pos % 2 == 1) {
            std::reverse(bnod[u].begin() + 1, bnod[u].end());
            return int(bnod[u].size()) - pos;
        }
        return pos;
    }

    void setMatch(int u, int v) {
        mat[u] = adj[u][v].v;
        if (u <= n) { return; }
        Edge edge = adj[u][v];
        int x = base[u][edge.u];
        int pos = getPosition(u, x);
        for (int i = 0; i < pos; i++) setMatch(bnod[u][i], bnod[u][i ^ 1]);
        setMatch(x, v);
        std::rotate(bnod[u].begin(), bnod[u].begin() + pos, bnod[u].end());
    }

    void augment(int u, int v) {
        int mtch = top[mat[u]];
        setMatch(u, v);
        if (!mtch) { return; }
        setMatch(mtch, top[par[mtch]]);
        augment(top[par[mtch]], mtch);
    }

    int lca(int u, int v) {
        for (tms++; u || v; std::swap(u, v)) {
            if (!u) { continue; }
            if (vtim[u] == tms) { return u; }
            vtim[u] = tms;
            u = top[mat[u]];
            if (u) { u = top[par[u]]; }
        }
        return 0;
    }

    void addBlossom(int u, int anc, int v) {
        int blo = n + 1;
        while (blo <= m && top[blo]) blo++;
        if (blo > m) { m++; }
        lab[blo] = 0;
        col[blo] = 0;
        top[blo] = 0;
        mat[blo] = mat[anc];
        bnod[blo].clear();
        bnod[blo].emplace_back(anc);
        int next;
        for (int i = u; i != anc; i = top[par[next]]) {
            bnod[blo].emplace_back(i);
            next = top[mat[i]];
            bnod[blo].emplace_back(next);
            enqueue(next);
        }
        std::reverse(bnod[blo].begin() + 1, bnod[blo].end());
        for (int i = v; i != anc; i = top[par[next]]) {
            bnod[blo].emplace_back(i);
            next = top[mat[i]];
            bnod[blo].emplace_back(next);
            enqueue(next);
        }
        setTop(blo, blo);
        for (int i = 1; i <= m; i++) adj[blo][i].w = adj[i][blo].w = 0;
        std::fill(base[blo].begin() + 1, base[blo].begin() + n + 1, 0);
        for (int x : bnod[blo]) {
            for (int y = 1; y <= m; y++) {
                if (!adj[blo][y].w || calcDelta(adj[x][y]) < calcDelta(adj[blo][y])) {
                    adj[blo][y] = adj[x][y];
                    adj[y][blo] = adj[y][x];
                }
            }
            for (int y = 1; y <= n; y++) {
                if (base[x][y]) { base[blo][y] = x; }
            }
        }
        setSlack(blo);
    }

    void expandBlossom(int blo) {
        for (int cur : bnod[blo]) setTop(cur, cur);
        int anc = base[blo][adj[blo][par[blo]].u];
        int pos = getPosition(blo, anc);
        for (int i = 0; i < pos; i += 2) {
            int x = bnod[blo][i], y = bnod[blo][i + 1];
            par[x] = adj[y][x].u;
            col[x] = 1;
            col[y] = 0;
            slk[x] = 0;
            setSlack(y);
            enqueue(y);
        }
        col[anc] = 1;
        par[anc] = par[blo];
        for (int i = pos + 1; i < int(bnod[blo].size()); i++) {
            col[bnod[blo][i]] = -1;
            setSlack(bnod[blo][i]);
        }
        top[blo] = 0;
    }

    bool processEdge(const Edge &edge) {
        int u = top[edge.u], v = top[edge.v];
        if (col[v] == -1) {
            par[v] = edge.u;
            col[v] = 1;
            int mtch = top[mat[v]];
            slk[v] = 0;
            slk[mtch] = 0;
            col[mtch] = 0;
            enqueue(mtch);
        } else if (!col[v]) {
            int anc = lca(u, v);
            if (!anc) {
                augment(u, v);
                augment(v, u);
                return true;
            }
            addBlossom(u, anc, v);
        }
        return false;
    }

    bool bfs() {
        std::fill(col.begin() + 1, col.begin() + m + 1, -1);
        std::fill(slk.begin() + 1, slk.begin() + m + 1, 0);
        while (!que.empty()) que.pop();
        for (int i = 1; i <= m; i++) {
            if (top[i] == i && !mat[i]) {
                par[i] = col[i] = 0;
                enqueue(i);
            }
        }
        if (que.empty()) { return false; }
        while (true) {
            while (!que.empty()) {
                int u = que.front();
                que.pop();
                if (col[top[u]] == 1) { continue; }
                for (int v = 1; v <= n; v++) {
                    if (adj[u][v].w > 0 && top[u] != top[v]) {
                        if (calcDelta(adj[u][v]) != 0) { updateSlack(u, top[v]); }
                        else if (processEdge(adj[u][v])) return true;
                    }
                }
            }
            T dt = inf<T>;
            for (int i = n + 1; i <= m; i++) {
                if (top[i] == i && col[i] == 1) { dt = std::min(dt, lab[i] / 2); }
            }
            for (int i = 1; i <= m; i++) {
                if (top[i] == i && slk[i] && col[i] != 1) {
                    T curd = calcDelta(adj[slk[i]][i]);
                    if (col[i] == -1) { dt = std::min(dt, curd); }
                    else if (col[i] == 0) dt = std::min(dt, curd / 2);
                }
            }
            for (int i = 1; i <= n; i++) {
                if (col[top[i]] != -1) {
                    lab[i] += (col[top[i]] * 2 - 1) * dt;
                    if (lab[i] <= 0) { return false; }
                }
            }
            for (int i = n + 1; i <= m; i++) {
                if (top[i] == i && col[top[i]] != -1) { lab[i] += (2 - col[top[i]] * 4) * dt; }
            }
            while (!que.empty()) que.pop();
            for (int i = 1; i <= m; i++) {
                if (top[i] == i && slk[i] && top[slk[i]] != i && calcDelta(adj[slk[i]][i]) == 0 && processEdge(adj[slk[i]][i])) { return true; }
            }
            for (int i = n + 1; i <= m; i++) {
                if (top[i] == i && col[i] == 1 && !lab[i]) { expandBlossom(i); }
            }
        }
    }

    std::pair<T, std::vector<int>> work(const std::vector<std::tuple<int, int, T>> &eds) {
        m = n;
        tms = 0;
        std::fill(vtim.begin(), vtim.end(), 0);
        std::fill(mat.begin(), mat.end(), 0);
        std::iota(top.begin() + 1, top.begin() + n + 1, 1);
        T maxw = 0;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) adj[i][j] = {i, j, 0};
        }
        for (auto [u, v, wgt] : eds) {
            maxw = std::max(maxw, wgt);
            adj[u][v].w = std::max(adj[u][v].w, wgt);
            adj[v][u].w = std::max(adj[v][u].w, wgt);
        }
        for (int i = 1; i <= n; i++) bnod[i].clear();
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) base[i][j] = i == j ? i : 0;
        }
        std::fill(lab.begin() + 1, lab.begin() + n + 1, maxw);
        while (bfs()) {}
        T res = 0;
        std::vector<int> mtch(n + 1);
        for (int i = 1; i <= n; i++) {
            if (mat[i]) {
                res += adj[i][mat[i]].w;
                mtch[i] = mat[i];
            }
        }
        return {res / 2, mtch};
    }
};
