#pragma once

#include <vector>

template <class Info, class Tag>
struct LazySegmentTree {
    int n;
    std::vector<Info> info;
    std::vector<Tag> tag;

    LazySegmentTree() : n(0) {}

    explicit LazySegmentTree(int _n, Info value = Info()) { init(_n, value); }

    explicit LazySegmentTree(const std::vector<Info> &values) { init(values); }

    void init(int _n, Info value = Info()) { init(std::vector<Info>(_n, value)); }

    void init(const std::vector<Info> &values) {
        n = int(values.size()) - 1;
        info.assign(n << 2 | 1, Info());
        tag.assign(n << 2 | 1, Tag());
        auto build = [&] (auto &&self, int p, int l, int r) -> void {
            if (l == r) {
                info[p] = values[l];
                return;
            }
            int mid = (l + r) >> 1;
            self(self, p << 1, l, mid);
            self(self, p << 1 | 1, mid + 1, r);
            pull(p);
        };

        build(build, 1, 1, n);
    }

    void pull(int p) { info[p] = info[p << 1] + info[p << 1 | 1]; }

    void apply(int p, const Tag &value) {
        info[p].apply(value);
        tag[p].apply(value);
    }

    void push(int p) {
        apply(p << 1, tag[p]);
        apply(p << 1 | 1, tag[p]);
        tag[p] = Tag();
    }

    void modify(int p, int l, int r, int x, const Info &value) {
        if (l == r) {
            info[p] = value;
            tag[p] = Tag();
            return ;
        }
        int mid = (l + r) >> 1;
        push(p);
        if (x <= mid) {
            modify(p << 1, l, mid, x, value);
        } else {
            modify(p << 1 | 1, mid + 1, r, x, value);
        }
        pull(p);
    }

    void modify(int x, const Info &value) {
        modify(1, 1, n, x, value);
    }

    void rangeModify(int p, int l, int r, int x, int y, const Tag &value) {
        if (r < x || l > y) { return; }
        if (x <= l && r <= y) {
            apply(p, value);
            return;
        }
        push(p);
        int mid = (l + r) >> 1;
        rangeModify(p << 1, l, mid, x, y, value);
        rangeModify(p << 1 | 1, mid + 1, r, x, y, value);
        pull(p);
    }

    void rangeModify(int l, int r, const Tag &value) {
        rangeModify(1, 1, n, l, r, value);
    }

    Info rangeQuery(int p, int l, int r, int x, int y) {
        if (r < x || l > y) { return Info(); }
        if (x <= l && r <= y) { return info[p]; }
        push(p);
        int mid = (l + r) >> 1;
        return rangeQuery(p << 1, l, mid, x, y)
            + rangeQuery(p << 1 | 1, mid + 1, r, x, y);
    }

    Info rangeQuery(int l, int r) {
        return rangeQuery(1, 1, n, l, r);
    }

    template <class F>
    int findFirst(int p, int l, int r, int x, int y, F pred) {
        if (l > y || r < x) { return -1; }
        if (l >= x && r <= y && !pred(info[p])) { return -1; }
        if (l == r) { return l; }
        push(p);
        int mid = (l + r) >> 1;
        int res = findFirst(p << 1, l, mid, x, y, pred);
        if (res == -1) { res = findFirst(p << 1 | 1, mid + 1, r, x, y, pred); }
        return res;
    }

    template <class F>
    int findFirst(int l, int r, F pred) {
        return findFirst(1, 1, n, l, r, pred);
    }

    template <class F>
    int findLast(int p, int l, int r, int x, int y, F pred) {
        if (l > y || r < x) { return -1; }
        if (l >= x && r <= y && !pred(info[p])) { return -1; }
        if (l == r) { return l; }
        push(p);
        int mid = (l + r) >> 1;
        int res = findLast(p << 1 | 1, mid + 1, r, x, y, pred);
        if (res == -1) { res = findLast(p << 1, l, mid, x, y, pred); }
        return res;
    }

    template <class F>
    int findLast(int l, int r, F pred) {
        return findLast(1, 1, n, l, r, pred);
    }
};
