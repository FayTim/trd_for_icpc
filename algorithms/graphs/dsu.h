#pragma once
#include <algorithm>
#include <numeric>
#include <vector>
using namespace std;
using ll = long long;

struct DSU {
    vector<int> p, sz;
    DSU(int n) : p(n), sz(n, 1) {
        iota(p.begin(), p.end(), 0);
    }
    int f(int x) {
        return p[x] == x ? x : p[x] = f(p[x]);
    }
    bool u(int a, int b) {  // true, если были в разных компонентах
        a = f(a);
        b = f(b);
        if (a == b) return 0;
        if (sz[a] < sz[b]) swap(a, b);
        p[b] = a;
        sz[a] += sz[b];
        return 1;
    }
};
