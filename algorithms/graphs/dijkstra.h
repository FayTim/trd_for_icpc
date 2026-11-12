#pragma once
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define all(x) (x).begin(), (x).end()
const ll INF = 4e18;

// g[v] = {to, w}, w >= 0. d[v] = INF если недостижима. par (необязательно) — для восстановления пути
vector<ll> dijkstra(int s, vector<vector<pair<int, int>>>& g, vector<int>* par = nullptr) {
    vector<ll> d(g.size(), INF);
    if (par) par->assign(g.size(), -1);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq;
    d[s] = 0; pq.push({0, s});
    while (!pq.empty()) {
        auto [dv, v] = pq.top();
        pq.pop();
        if (dv > d[v]) continue;
        for (auto [to, w] : g[v]) {
            if (d[to] > dv + w) {
                d[to] = dv + w;
                if (par) (*par)[to] = v;
                pq.push({d[to], to});
            }
        }
    }
    return d;
}