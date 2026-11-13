#pragma once
#include <functional>
#include <queue>
#include <utility>
#include <vector>
#include "inf.h"

namespace dijkstra_h {
using namespace std;
using ll = long long;
using graph_constants::INF;

// g[v] = {to, w}, w >= 0. d[v] = INF если недостижима. par (необязательно) — для восстановления пути
inline vector<ll> dijkstra(int s, vector<vector<pair<int, ll>>>& g, vector<int>* par = nullptr) {
    vector<ll> d(g.size(), INF);
    if (par) par->assign(g.size(), -1);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq;
    d[s] = 0; pq.push({0, s});
    while (!pq.empty()) {
        auto [dv, v] = pq.top();
        pq.pop();
        if (dv > d[v]) continue;
        for (auto [to, w] : g[v]) {
            if (w >= INF - dv) continue;  // сумма >= INF, без переполнения
            if (d[to] > dv + w) {
                d[to] = dv + w;
                if (par) (*par)[to] = v;
                pq.push({d[to], to});
            }
        }
    }
    return d;
}

}  // namespace dijkstra_h
