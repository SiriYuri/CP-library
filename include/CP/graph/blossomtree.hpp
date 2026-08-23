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

    int n, m, timeStamp;
    std::vector<std::vector<Edge>> adjacency;
    std::vector<std::vector<int>> base, blossomNodes;
    std::vector<T> label;
    std::vector<int> match, slack, top, parent, color, visitedTime;
    std::queue<int> queue;

    explicit BlossomTree(int _n) : n(_n - 1) {
        int size = 2 * n + 2;
        adjacency.assign(size, std::vector<Edge>(size));
        base.assign(size, std::vector<int>(size));
        blossomNodes.resize(size);
        label.assign(size, 0);
        match.assign(size, 0);
        slack.assign(size, 0);
        top.assign(size, 0);
        parent.assign(size, 0);
        color.assign(size, 0);
        visitedTime.assign(size, 0);
    }

    T calcDelta(const Edge &edge) const { return label[edge.u] + label[edge.v] - edge.w * 2; }

    void updateSlack(int u, int v) {
        if (!slack[v] || calcDelta(adjacency[u][v]) < calcDelta(adjacency[slack[v]][v])) slack[v] = u;
    }

    void setSlack(int v) {
        slack[v] = 0;
        for (int u = 1; u <= n; u++) {
            if (adjacency[u][v].w > 0 && top[u] != v && !color[top[u]]) updateSlack(u, v);
        }
    }

    void enqueue(int u) {
        if (u <= n) queue.emplace(u);
        else for (int v : blossomNodes[u]) enqueue(v);
    }

    void setTop(int u, int value) {
        top[u] = value;
        if (u > n) {
            for (int v : blossomNodes[u]) setTop(v, value);
        }
    }

    int getPosition(int u, int v) {
        int position = int(std::find(blossomNodes[u].begin(), blossomNodes[u].end(), v) - blossomNodes[u].begin());
        if (position % 2 == 1) {
            std::reverse(blossomNodes[u].begin() + 1, blossomNodes[u].end());
            return int(blossomNodes[u].size()) - position;
        }
        return position;
    }

    void setMatch(int u, int v) {
        match[u] = adjacency[u][v].v;
        if (u <= n) return;
        Edge edge = adjacency[u][v];
        int x = base[u][edge.u];
        int position = getPosition(u, x);
        for (int i = 0; i < position; i++) setMatch(blossomNodes[u][i], blossomNodes[u][i ^ 1]);
        setMatch(x, v);
        std::rotate(blossomNodes[u].begin(), blossomNodes[u].begin() + position, blossomNodes[u].end());
    }

    void augment(int u, int v) {
        int matched = top[match[u]];
        setMatch(u, v);
        if (!matched) return;
        setMatch(matched, top[parent[matched]]);
        augment(top[parent[matched]], matched);
    }

    int lca(int u, int v) {
        for (timeStamp++; u || v; std::swap(u, v)) {
            if (!u) continue;
            if (visitedTime[u] == timeStamp) return u;
            visitedTime[u] = timeStamp;
            u = top[match[u]];
            if (u) u = top[parent[u]];
        }
        return 0;
    }

    void addBlossom(int u, int lcaNode, int v) {
        int blossom = n + 1;
        while (blossom <= m && top[blossom]) blossom++;
        if (blossom > m) m++;
        label[blossom] = 0;
        color[blossom] = 0;
        top[blossom] = 0;
        match[blossom] = match[lcaNode];
        blossomNodes[blossom].clear();
        blossomNodes[blossom].emplace_back(lcaNode);
        int next;
        for (int i = u; i != lcaNode; i = top[parent[next]]) {
            blossomNodes[blossom].emplace_back(i);
            next = top[match[i]];
            blossomNodes[blossom].emplace_back(next);
            enqueue(next);
        }
        std::reverse(blossomNodes[blossom].begin() + 1, blossomNodes[blossom].end());
        for (int i = v; i != lcaNode; i = top[parent[next]]) {
            blossomNodes[blossom].emplace_back(i);
            next = top[match[i]];
            blossomNodes[blossom].emplace_back(next);
            enqueue(next);
        }
        setTop(blossom, blossom);
        for (int i = 1; i <= m; i++) adjacency[blossom][i].w = adjacency[i][blossom].w = 0;
        std::fill(base[blossom].begin() + 1, base[blossom].begin() + n + 1, 0);
        for (int x : blossomNodes[blossom]) {
            for (int y = 1; y <= m; y++) {
                if (!adjacency[blossom][y].w || calcDelta(adjacency[x][y]) < calcDelta(adjacency[blossom][y])) {
                    adjacency[blossom][y] = adjacency[x][y];
                    adjacency[y][blossom] = adjacency[y][x];
                }
            }
            for (int y = 1; y <= n; y++) {
                if (base[x][y]) base[blossom][y] = x;
            }
        }
        setSlack(blossom);
    }

    void expandBlossom(int blossom) {
        for (int current : blossomNodes[blossom]) setTop(current, current);
        int lcaNode = base[blossom][adjacency[blossom][parent[blossom]].u];
        int position = getPosition(blossom, lcaNode);
        for (int i = 0; i < position; i += 2) {
            int x = blossomNodes[blossom][i], y = blossomNodes[blossom][i + 1];
            parent[x] = adjacency[y][x].u;
            color[x] = 1;
            color[y] = 0;
            slack[x] = 0;
            setSlack(y);
            enqueue(y);
        }
        color[lcaNode] = 1;
        parent[lcaNode] = parent[blossom];
        for (int i = position + 1; i < int(blossomNodes[blossom].size()); i++) {
            color[blossomNodes[blossom][i]] = -1;
            setSlack(blossomNodes[blossom][i]);
        }
        top[blossom] = 0;
    }

    bool processEdge(const Edge &edge) {
        int u = top[edge.u], v = top[edge.v];
        if (color[v] == -1) {
            parent[v] = edge.u;
            color[v] = 1;
            int matched = top[match[v]];
            slack[v] = 0;
            slack[matched] = 0;
            color[matched] = 0;
            enqueue(matched);
        } else if (!color[v]) {
            int lcaNode = lca(u, v);
            if (!lcaNode) {
                augment(u, v);
                augment(v, u);
                return true;
            }
            addBlossom(u, lcaNode, v);
        }
        return false;
    }

    bool bfs() {
        std::fill(color.begin() + 1, color.begin() + m + 1, -1);
        std::fill(slack.begin() + 1, slack.begin() + m + 1, 0);
        while (!queue.empty()) queue.pop();
        for (int i = 1; i <= m; i++) {
            if (top[i] == i && !match[i]) {
                parent[i] = color[i] = 0;
                enqueue(i);
            }
        }
        if (queue.empty()) return false;
        while (true) {
            while (!queue.empty()) {
                int u = queue.front();
                queue.pop();
                if (color[top[u]] == 1) continue;
                for (int v = 1; v <= n; v++) {
                    if (adjacency[u][v].w > 0 && top[u] != top[v]) {
                        if (calcDelta(adjacency[u][v]) != 0) updateSlack(u, top[v]);
                        else if (processEdge(adjacency[u][v])) return true;
                    }
                }
            }
            T delta = inf<T>;
            for (int i = n + 1; i <= m; i++) {
                if (top[i] == i && color[i] == 1) delta = std::min(delta, label[i] / 2);
            }
            for (int i = 1; i <= m; i++) {
                if (top[i] == i && slack[i] && color[i] != 1) {
                    T currentDelta = calcDelta(adjacency[slack[i]][i]);
                    if (color[i] == -1) delta = std::min(delta, currentDelta);
                    else if (color[i] == 0) delta = std::min(delta, currentDelta / 2);
                }
            }
            for (int i = 1; i <= n; i++) {
                if (color[top[i]] != -1) {
                    label[i] += (color[top[i]] * 2 - 1) * delta;
                    if (label[i] <= 0) return false;
                }
            }
            for (int i = n + 1; i <= m; i++) {
                if (top[i] == i && color[top[i]] != -1) label[i] += (2 - color[top[i]] * 4) * delta;
            }
            while (!queue.empty()) queue.pop();
            for (int i = 1; i <= m; i++) {
                if (top[i] == i && slack[i] && top[slack[i]] != i && calcDelta(adjacency[slack[i]][i]) == 0 && processEdge(adjacency[slack[i]][i])) return true;
            }
            for (int i = n + 1; i <= m; i++) {
                if (top[i] == i && color[i] == 1 && !label[i]) expandBlossom(i);
            }
        }
    }

    std::pair<T, std::vector<int>> work(const std::vector<std::tuple<int, int, T>> &edges) {
        m = n;
        timeStamp = 0;
        std::fill(visitedTime.begin(), visitedTime.end(), 0);
        std::fill(match.begin(), match.end(), 0);
        std::iota(top.begin() + 1, top.begin() + n + 1, 1);
        T maximumWeight = 0;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) adjacency[i][j] = {i, j, 0};
        }
        for (auto [u, v, weight] : edges) {
            maximumWeight = std::max(maximumWeight, weight);
            adjacency[u][v].w = std::max(adjacency[u][v].w, weight);
            adjacency[v][u].w = std::max(adjacency[v][u].w, weight);
        }
        for (int i = 1; i <= n; i++) blossomNodes[i].clear();
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) base[i][j] = i == j ? i : 0;
        }
        std::fill(label.begin() + 1, label.begin() + n + 1, maximumWeight);
        while (bfs()) {}
        T result = 0;
        std::vector<int> matching(n + 1);
        for (int i = 1; i <= n; i++) {
            if (match[i]) {
                result += adjacency[i][match[i]].w;
                matching[i] = match[i];
            }
        }
        return {result / 2, matching};
    }
};
