#pragma once

#include <functional>
#include <optional>
#include <utility>
#include <vector>

template <class T, class Compare = std::less<T>>
struct DynHeap {
    int n, total;
    Compare compare;
    std::vector<T> value;
    std::vector<int> left, right, distance;
    std::vector<int> parent, count;
    std::vector<int> deleted;

    explicit DynHeap(int _n, Compare _compare = Compare()) : n(_n - 1), total(0), compare(_compare), value(_n), left(_n), right(_n), distance(_n), parent(_n), count(_n), deleted(_n) {}

    int insert(const T &x) {
        int id = ++total;
        value[id] = x;
        left[id] = right[id] = deleted[id] = 0;
        distance[id] = count[id] = 1;
        parent[id] = id;
        return id;
    }

    int find(int x) { return parent[x] == x ? x : parent[x] = find(parent[x]); }

    bool less(int x, int y) const {
        if (compare(value[x], value[y])) { return true; }
        if (compare(value[y], value[x])) { return false; }
        return x < y;
    }

    int meld(int x, int y) {
        if (!x || !y) { return x | y; }
        if (less(y, x)) { std::swap(x, y); }
        right[x] = meld(right[x], y);
        if (distance[left[x]] < distance[right[x]]) { std::swap(left[x], right[x]); }
        distance[x] = distance[right[x]] + 1;
        return x;
    }

    bool merge(int x, int y) {
        if (deleted[x] || deleted[y]) { return false; }
        x = find(x);
        y = find(y);
        if (x == y) { return false; }
        int root = meld(x, y);
        count[root] = count[x] + count[y];
        parent[x] = parent[y] = parent[root] = root;
        return true;
    }

    bool erased(int x) const { return deleted[x]; }

    int size(int x) {
        if (deleted[x]) { return 0; }
        return count[find(x)];
    }

    std::optional<std::pair<T, int>> top(int x) {
        if (deleted[x]) { return std::nullopt; }
        int root = find(x);
        return std::pair<T, int> {value[root], root};
    }

    std::optional<std::pair<T, int>> pop(int x) {
        if (deleted[x]) { return std::nullopt; }
        int root = find(x);
        std::pair<T, int> result {value[root], root};
        deleted[root] = 1;
        int newRoot = meld(left[root], right[root]);
        if (newRoot) {
            parent[newRoot] = newRoot;
            count[newRoot] = count[root] - 1;
        }
        parent[root] = newRoot;
        left[root] = right[root] = count[root] = 0;
        return result;
    }
};
