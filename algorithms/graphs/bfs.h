#pragma once
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define all(x) (x).begin(), (x).end()

// d[v] = расстояние до ближайшего источника (-1 если нет пути), par[v] = предок (-1 у источников)
vector<int> bfs(const vector<int>& src, vector<vector<int>>& g, vector<int>& par) {
    vector<int> d(g.size(), -1);
    par.assign(g.size(), -1);
    queue<int> q;
    for (int s : src) d[s] = 0, q.push(s);
    while (!q.empty()) {
        int v = q.front(); q.pop();
        for (int to : g[v]) {
            if (d[to] < 0) {
                d[to] = d[v] + 1;
                par[to] = v;
                q.push(to);
            }
        }
    }
    return d;
}
vector<int> path(int t, vector<int>& d, vector<int>& par) {
    if (d[t] < 0) return {};
    vector<int> p;
    for (int v = t; v != -1; v = par[v]) p.push_back(v);
    reverse(all(p));
    return p;
}

// 0-1 BFS: g[v] = {to, w}, w in {0, 1}; недостижимые = INT_MAX
vector<int> bfs01(int s, vector<vector<pair<int, int>>>& g) {
    vector<int> d(g.size(), INT_MAX);
    deque<int> q = {s}; d[s] = 0;
    while (!q.empty()) {
        int v = q.front(); q.pop_front();
        for (auto [to, w] : g[v]) {
            if (d[to] > d[v] + w) {
                d[to] = d[v] + w;
                if (w) q.push_back(to);
                else q.push_front(to);
            }
        }
    }
    return d;
}