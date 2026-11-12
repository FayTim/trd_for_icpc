#pragma once
#include <bits/stdc++.h>
#include "inf.h"
using namespace std;
using ll = long long;
#define all(x) (x).begin(), (x).end()

// d[i][i] = 0, d[i][j] = w (min по кратным), INF если ребра нет
// после: d[i][j] — расстояние; d[i][i] < 0 — i на отрицательном цикле
void floyd(vector<vector<ll>>& d) {
    int n = d.size();
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++) if (d[i][k] < INF)
            for (int j = 0; j < n; j++) 
                if (d[k][j] < INF) {
					__int128 x = (__int128)d[i][k] + d[k][j];
					x = max(x, -(__int128)INF);
					x = min(x, (__int128)INF);

					if (x < d[i][j])
    					d[i][j] = (ll)x;
                }
            
}