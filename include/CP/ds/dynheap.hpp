#pragma once

#include <functional>
#include <optional>
#include <utility>
#include <vector>

template <class T, class Compare = std::less<T>>
struct DynHeap {
    int n, tot;
    Compare cmp;
    std::vector<T> val;
    std::vector<int> left, rgt, dist;
    std::vector<int> par, cnt;
    std::vector<int> del;

    explicit DynHeap(int _n, Compare _cmp = Compare()) : n(_n - 1), tot(0), cmp(_cmp), val(_n), left(_n), rgt(_n), dist(_n), par(_n), cnt(_n), del(_n) {}

    int insert(const T &x) {
        int id = ++tot;
        val[id] = x;
        left[id] = rgt[id] = del[id] = 0;
        dist[id] = cnt[id] = 1;
        par[id] = id;
        return id;
    }

    int find(int x) { return par[x] == x ? x : par[x] = find(par[x]); }

    bool less(int x, int y) const {
        if (cmp(val[x], val[y])) { return true; }
        if (cmp(val[y], val[x])) { return false; }
        return x < y;
    }

    int meld(int x, int y) {
        if (!x || !y) { return x | y; }
        if (less(y, x)) { std::swap(x, y); }
        rgt[x] = meld(rgt[x], y);
        if (dist[left[x]] < dist[rgt[x]]) { std::swap(left[x], rgt[x]); }
        dist[x] = dist[rgt[x]] + 1;
        return x;
    }

    bool merge(int x, int y) {
        if (del[x] || del[y]) { return false; }
        x = find(x);
        y = find(y);
        if (x == y) { return false; }
        int root = meld(x, y);
        cnt[root] = cnt[x] + cnt[y];
        par[x] = par[y] = par[root] = root;
        return true;
    }

    bool erased(int x) const { return del[x]; }

    int size(int x) {
        if (del[x]) { return 0; }
        return cnt[find(x)];
    }

    std::optional<std::pair<T, int>> top(int x) {
        if (del[x]) { return std::nullopt; }
        int root = find(x);
        return std::pair<T, int> {val[root], root};
    }

    std::optional<std::pair<T, int>> pop(int x) {
        if (del[x]) { return std::nullopt; }
        int root = find(x);
        std::pair<T, int> res {val[root], root};
        del[root] = 1;
        int nrt = meld(left[root], rgt[root]);
        if (nrt) {
            par[nrt] = nrt;
            cnt[nrt] = cnt[root] - 1;
        }
        par[root] = nrt;
        left[root] = rgt[root] = cnt[root] = 0;
        return res;
    }
};
