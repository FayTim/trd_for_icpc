#pragma once
#include <algorithm>
#include <array>
#include <utility>
#include <vector>
#include "dsu.h"
using namespace std;
using ll = long long;

// e[i] = {w, a, b}; возвращает {вес остова (леса), число рёбер в нём}
inline pair<ll, int> kruskal(int n, vector<array<ll, 3>> e) {
    sort(e.begin(), e.end());
    DSU d(n);
    __int128 res = 0; int cnt = 0;  // промежуточные суммы могут выходить за ll
    for (auto [w, a, b] : e) {
        if (d.u(a, b)) {
            res += w;
            cnt++;
        }
    }
    return {static_cast<ll>(res), cnt};  // итоговый вес должен помещаться в ll
}
