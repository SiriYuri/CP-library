#pragma once

#include <string>
#include <tuple>
#include <vector>

inline auto kmp(std::string s, std::string t) {
    int n = int(s.size()), m = int(t.size());
    std::vector<int> fail(m);
    std::vector<int> res;
    if (m == 0) {
        for (int i = 0; i <= n; i++) {
            res.emplace_back(i);
        }
        return std::make_tuple(res, fail);
    }
    for (int i = 1, j = 0; i < m; i++) {
        while (j && t[i] != t[j]) j = fail[j - 1];
        if (t[i] == t[j]) j++;
        fail[i] = j;
    }
    for (int i = 0, j = 0; i < n; i++) {
        while (j && s[i] != t[j]) j = fail[j - 1];
        if (s[i] == t[j]) j++;
        if (j == m) {
            res.emplace_back(i - m + 1);
            j = fail[j - 1];
        }
    }
    return std::make_tuple(res, fail);
}
