#pragma once

#include <utility>
#include <vector>

template <class T, class Op>
struct SparseTable {
    int n, m;
    std::vector<int> log;
    std::vector<std::vector<T>> st;
    Op op;

    SparseTable() = default;

    explicit SparseTable(const std::vector<T> &vals, Op op = Op()) : op(std::move(op)) {
        init(vals);
    }

    void init(const std::vector<T> &vals) {
        n = int(vals.size()) - 1;
        log.assign(n + 1, 0);
        for (int i = 2; i <= n; ++i) log[i] = log[i >> 1] + 1;
        m = log[n] + 1;
        st.assign(m, std::vector<T>(n + 1));
        st[0] = vals;
        for (int j = 1; j < m; ++j) {
            int half = 1 << (j - 1), len = 1 << j;
            for (int i = 1; i + len - 1 <= n; ++i) {
                st[j][i] = op(st[j - 1][i], st[j - 1][i + half]);
            }
        }
    }

    T query(int l, int r) const {
        int k = log[r - l + 1];
        return op(st[k][l], st[k][r - (1 << k) + 1]);
    }
};
