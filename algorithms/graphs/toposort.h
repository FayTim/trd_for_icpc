#pragma once
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define all(x) (x).begin(), (x).end()

// порядок вершин ориентированного графа; пустой вектор, если есть цикл
vector<int> toposort(vector<vector<int>>& g) {
    int n = g.size();
    vector<int> deg(n), res;
    for (int v = 0; v < n; v++)
        for (int to : g[v])
            deg[to]++;
    queue<int> q;
    for (int i = 0; i < n; i++)
        if (!deg[i])
            q.push(i);
    while (!q.empty()) {
        int v = q.front(); q.pop();
        res.push_back(v);
        for (int to : g[v])
            if (!--deg[to])
                q.push(to);
    }
    if ((int)res.size() != n) res.clear();
    return res;
}