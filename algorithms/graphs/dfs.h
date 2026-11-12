#pragma once
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int n, T;
vector<vector<int>> g;
vector<int> tin, tout;

void dfs(int v) {
    tin[v] = T++;
    for (int to : g[v]) if (tin[to] < 0) dfs(to);
    tout[v] = T++;
}
bool anc(int a, int b) { return tin[a] <= tin[b] && tout[b] <= tout[a]; }

// компоненты связности (неориентированный), итеративно
int components(vector<int>& comp) {
    comp.assign(n, -1); int c = 0;
    for (int s = 0; s < n; s++) if (comp[s] < 0) {
        vector<int> st = {s};
        comp[s] = c;
        while (!st.empty()) {
            int v = st.back(); st.pop_back();
            for (int to : g[v]) {
                if (comp[to] < 0) {
                    comp[to] = c,
                    st.push_back(to);
                }
            }
        }
        c++;
    }
    return c;
}

bool cyc(int v, vector<int>& col) {
    col[v] = 1;
    for (int to : g[v]) {
        if (col[to] == 1) return 1;
        if (!col[to] && cyc(to, col)) return 1;
    }
    col[v] = 2;
    return 0;
}