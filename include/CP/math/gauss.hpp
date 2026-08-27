#pragma once

#include "CP/types.hpp"

#include <algorithm>
#include <cmath>
#include <tuple>
#include <vector>

template <class T>
auto gauss(std::vector<std::vector<T>> func) {
    int n = int(func.size()) - 1, m = int(func[1].size()) - 1, vars = m - 1;
    int row = 1, col = 1;
    std::vector<int> prow(vars + 1);
    while (row <= n && col <= vars) {
        int sel = row;
        for (int i = row + 1; i <= n; i++) {
            if (std::abs(func[i][col]) > std::abs(func[sel][col])) { sel = i; }
        }
        if (std::abs(func[sel][col]) < EPS) {
            col++;
            continue;
        }
        std::swap(func[row], func[sel]);
        prow[col] = row;
        T div = func[row][col];
        for (int j = col; j <= m; j++) func[row][j] /= div;
        for (int i = 1; i <= n; i++) {
            if (i != row && std::abs(func[i][col]) > EPS) {
                T fac = func[i][col];
                for (int j = col; j <= m; j++) func[i][j] -= fac * func[row][j];
            }
        }
        row++;
        col++;
    }
    for (int i = 1; i <= n; i++) {
        bool zero = true;
        for (int j = 1; j <= vars; j++) {
            if (std::abs(func[i][j]) > EPS) {
                zero = false;
                break;
            }
        }
        if (zero && std::abs(func[i][m]) > EPS) { return std::make_tuple(0, std::vector<T>{}); }
    }
    int rank = row - 1;
    std::vector<T> ans(vars);
    for (int j = 1; j <= vars; j++) {
        if (prow[j]) { ans[j - 1] = func[prow[j]][m]; }
    }
    if (rank < vars) { return std::make_tuple(2, ans); }
    return std::make_tuple(1, ans);
}
