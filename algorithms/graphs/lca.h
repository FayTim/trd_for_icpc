#pragma once
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define all(x) (x).begin(), (x).end()

const int LOG = 18;  // n <= 2^LOG
int n;
vector<vector<int>> g;
vector<int> dep;
vector<array<int, LOG>> up;
void dfs(int v, int p) {
    up[v][0] = p;
    for (int i = 1; i < LOG; i++)
        up[v][i] = up[up[v][i - 1]][i - 1];
    for (int to : g[v])
        if (to != p) {
            dep[to] = dep[v] + 1;
            dfs(to, v);
        }
}
void build(int root = 0) {
    dep.assign(n, 0);
    up.assign(n, array<int, LOG>{});
    dfs(root, root);
}
int kth(int v, int k) {  // k-й предок; при k > dep[v] — корень
    for (int i = 0; i < LOG; i++)
        if (k >> i & 1)
            v = up[v][i];
    return v;
}
int lca(int a, int b) {
    if (dep[a] < dep[b]) swap(a, b);
    a = kth(a, dep[a] - dep[b]);
    if (a == b) return a;
    for (int i = LOG - 1; i >= 0; i--)
        if (up[a][i] != up[b][i]) {
            a = up[a][i];
            b = up[b][i];
        }
    return up[a][0];
}
int dist(int a, int b) {
    return dep[a] + dep[b] - 2 * dep[lca(a, b)];
}