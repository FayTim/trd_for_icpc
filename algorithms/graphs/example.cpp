#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define all(x) (x).begin(), (x).end()
const ll INF = 4e18;

#include <cmath>
#include <iostream>

#include "dsu.h"
#include "kruskal.h"
#include "ternary.h"

// В разных файлах совпадают имена (n, g, dfs, ...): подключаем их в свои пространства имён.
namespace dfs_h {
#include "dfs.h"
}
namespace bfs_h {
#include "bfs.h"
}
namespace dijkstra_h {
#include "dijkstra.h"
}
namespace bellman_h {
#include "bellman.h"
}
namespace floyd_h {
#include "floyd.h"
}
namespace toposort_h {
#include "toposort.h"
}
namespace bridges_h {
#include "bridges.h"
}
namespace lca_h {
#include "lca.h"
}

signed main() {
  {  // DFS
    dfs_h::n = 5;
    dfs_h::g = {{1}, {0}, {3}, {2}, {}};
    std::vector<int> comp;
    std::cout << dfs_h::components(comp) << '\n';  // 3
  }
  {  // BFS
    std::vector<std::vector<int>> g = {{1, 2}, {3}, {3}, {}};
    std::vector<int> par;
    auto d = bfs_h::bfs({0}, g, par);
    std::cout << d[3] << '\n';  // 2
    for (int v : bfs_h::path(3, d, par)) std::cout << v << ' ';  // 0 1 3
    std::cout << '\n';
  }
  {  // Дейкстра
    std::vector<std::vector<std::pair<int, int>>> g = {
        {{1, 4}, {2, 1}}, {{3, 1}}, {{1, 2}, {3, 5}}, {}};
    std::cout << dijkstra_h::dijkstra(0, g)[3] << '\n';  // 4
  }
  {  // Беллман-Форд
    std::vector<std::array<ll, 3>> e = {{0ll, 1ll, 4ll}, {1ll, 2ll, -2ll}, {0ll, 2ll, 5ll}};
    std::vector<ll> d;
    std::cout << bellman_h::bellman(3, 0, e, d) << ' ' << d[2] << '\n';  // 0 2
  }
  {  // Флойд
    std::vector<std::vector<ll>> d = {{0, 3, INF}, {INF, 0, 2}, {1, INF, 0}};
    floyd_h::floyd(d);
    std::cout << d[0][2] << '\n';  // 5
  }
  {  // Топсорт
    std::vector<std::vector<int>> g = {{1, 2}, {3}, {3}, {}};
    for (int v : toposort_h::toposort(g)) std::cout << v << ' ';  // 0 1 2 3
    std::cout << '\n';
  }
  {  // DSU
    DSU d(4);
    std::cout << d.u(0, 1) << d.u(1, 0) << ' ' << d.sz[d.f(0)] << '\n';  // 10 2
  }
  {  // Краскал
    auto [weight, edges] = kruskal(3, {{1, 0, 1}, {2, 1, 2}, {3, 0, 2}});
    std::cout << weight << ' ' << edges << '\n';  // 3 2
  }
  {  // Мосты и точки сочленения
    bridges_h::n = 4;
    bridges_h::g.assign(4, {});
    int id = 0;
    for (auto [a, b] : std::vector<std::pair<int, int>>{{0, 1}, {1, 2}, {2, 0}, {2, 3}}) {
      bridges_h::g[a].push_back({b, id});
      bridges_h::g[b].push_back({a, id});
      ++id;
    }
    bridges_h::run();
    std::cout << bridges_h::bridges[0] << ' ' << bridges_h::cut[2] << '\n';  // 3 1
  }
  {  // LCA
    lca_h::n = 5;
    lca_h::g = {{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    lca_h::build(0);
    std::cout << lca_h::lca(3, 4) << ' ' << lca_h::dist(3, 2) << '\n';  // 1 3
  }
  {  // Тернарный поиск
    std::cout << std::round(ternMin(0, 10, [](double x) { return (x - 3) * (x - 3); })) << '\n';  // 3
    std::cout << ternMinInt(0, 10, [](int i) { return (i - 4) * (i - 4); }) << '\n';              // 4
  }
}