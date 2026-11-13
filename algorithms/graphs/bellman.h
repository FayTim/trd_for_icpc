#pragma once
#include <array>
#include <queue>
#include <vector>
#include "inf.h"

namespace bellman_h {
using namespace std;
using ll = long long;
using graph_constants::INF;

// e[i] = {a, b, w} (ориентированное ребро a -> b), w любой знак
// true: из s достижим отрицательный цикл (d неверны); иначе d[v] = расстояние (INF если недостижима)
inline bool bellman(int n, int s, vector<array<ll, 3>>& e, vector<ll>& d) {
    vector<__int128> dist(n, INF);
    dist[s] = 0;

    vector<int> bad;

    for (int i = 0; i < n; i++) {
        for (auto [a, b, w] : e) {
            if (dist[a] == INF) continue;

            if (dist[b] > dist[a] + w) {
                dist[b] = dist[a] + w;

                if (i == n - 1)
                    bad.push_back(b);
            }
        }
    }

    vector<vector<int>> g(n);
    for (auto [a, b, w] : e)
        g[a].push_back(b);

    vector<bool> used(n, false);
    queue<int> q;

    for (int v : bad) {
        if (!used[v]) {
            used[v] = true;
            q.push(v);
        }
    }

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int to : g[v]) {
            if (!used[to]) {
                used[to] = true;
                q.push(to);
            }
        }
    }

    d.resize(n);

    for (int v = 0; v < n; v++) {
        if (used[v]) d[v] = -INF;
        else if (dist[v] == INF) d[v] = INF;
        else {
    		if (dist[v] < -(__int128)INF) d[v] = -INF;
    		else if (dist[v] > (__int128)INF) d[v] = INF;
    		else d[v] = (ll)dist[v];
		}
    }

    return !bad.empty();
}

}  // namespace bellman_h
