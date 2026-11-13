#pragma once
#include <algorithm>
#include <utility>
#include <vector>

namespace bridges_h {
using namespace std;
using ll = long long;

inline int n, T;
inline vector<vector<pair<int, int>>> g;  // g[v] = {to, id}; id 0..m-1 одинаков для обоих направлений
inline vector<int> tin, low, bridges;     // bridges: id рёбер-мостов
inline vector<char> cut;                  // cut[v]: точка сочленения
inline void dfs(int v, int pe) {
    tin[v] = low[v] = T++;
    int ch = 0;
    for (auto [to, id] : g[v]) {
        if (id == pe) continue;
        if (tin[to] >= 0) low[v] = min(low[v], tin[to]);
        else {
            dfs(to, id); ch++;
            low[v] = min(low[v], low[to]);
            if (low[to] > tin[v]) bridges.push_back(id);
            if (low[to] >= tin[v] && pe != -1) cut[v] = 1;
        }
    }
    if (pe == -1 && ch > 1) cut[v] = 1;
}
inline void run() {
    T = 0; tin.assign(n, -1); low.assign(n, 0); cut.assign(n, 0); bridges.clear();
    for (int v = 0; v < n; v++) if (tin[v] < 0) dfs(v, -1);
}

}  // namespace bridges_h
