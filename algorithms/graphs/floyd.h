#pragma once
#include <algorithm>
#include <vector>
#include "inf.h"

namespace floyd_h {
using namespace std;
using ll = long long;
using graph_constants::INF;

// d[i][i] = 0, d[i][j] = w (min по кратным), INF если ребра нет
// d[i][j] — расстояние лишь для пар без достижимого по пути отрицательного цикла.
// d[t][t] < 0 обнаруживает такой цикл; все плохие пары маркируйте отдельно.
inline void floyd(vector<vector<ll>>& d) {
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

}  // namespace floyd_h
