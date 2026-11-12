#pragma once
#include <bits/stdc++.h>
#include "dsu.h"
using namespace std;
using ll = long long;
#define all(x) (x).begin(), (x).end()

// e[i] = {w, a, b}; возвращает {вес остова (леса), число рёбер в нём}
pair<int, int> kruskal(int n, vector<array<int, 3>> e) {
    sort(all(e));
    DSU d(n);
    int res = 0; int cnt = 0;
    for (auto [w, a, b] : e) {
        if (d.u(a, b)) {
            res += w;
            cnt++;
        }
    }
    return {res, cnt};
}