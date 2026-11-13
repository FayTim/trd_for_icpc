#include "inf.h"
#include <iostream>
#include <cmath>
#include <climits>
#include <random>
#include <set>
#include <stdexcept>

#include "dsu.h"
#include "kruskal.h"
#include "ternary.h"

#include "dfs.h"
#include "bfs.h"
#include "dijkstra.h"
#include "bellman.h"
#include "floyd.h"
#include "toposort.h"
#include "bridges.h"
#include "lca.h"
#include "euler.h"

namespace {

using graph_constants::INF;
using Matrix = std::vector<std::vector<ll>>;
using Edges = std::vector<std::array<ll, 3>>;
using Graph = std::vector<std::vector<int>>;
using WeightedGraph = std::vector<std::vector<std::pair<int, ll>>>;

std::mt19937 random_engine(20260512);

int Rand(int lo, int hi) {
  return std::uniform_int_distribution<int>(lo, hi)(random_engine);
}

void Require(bool condition, const char* message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

// Матрица кратчайших рёбер: INF, если ребра нет, 0 на диагонали.
Matrix BuildMatrix(int n, const Edges& edges) {
  Matrix d(n, std::vector<ll>(n, INF));
  for (int i = 0; i < n; ++i) d[i][i] = 0;
  for (auto [a, b, w] : edges) d[a][b] = std::min(d[a][b], w);
  return d;
}

WeightedGraph ToWeighted(int n, const Edges& edges) {
  WeightedGraph g(n);
  for (auto [a, b, w] : edges) g[a].push_back({static_cast<int>(b), w});
  return g;
}

// reach[i][j]: есть путь из i в j хотя бы из одного ребра; reach[i][i] — i на цикле.
std::vector<std::vector<char>> Closure(const Graph& g) {
  int n = g.size();
  std::vector<std::vector<char>> reach(n, std::vector<char>(n, 0));
  for (int v = 0; v < n; ++v) for (int to : g[v]) reach[v][to] = 1;
  for (int k = 0; k < n; ++k)
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j)
        if (reach[i][k] && reach[k][j]) reach[i][j] = 1;
  return reach;
}

Edges RandomEdges(int n, int m, int min_w, int max_w) {
  Edges edges;
  for (int i = 0; i < m; ++i) {
    edges.push_back({Rand(0, n - 1), Rand(0, n - 1), Rand(min_w, max_w)});
  }
  return edges;
}

void TestDfs() {
  dfs_h::n = 5;
  dfs_h::g = {{1}, {0}, {3}, {2}, {}};
  std::vector<int> comp;
  Require(dfs_h::components(comp) == 3, "Component count differs");
  Require(comp[0] == comp[1] && comp[2] == comp[3], "Connected vertices must share a component");
  Require(comp[0] != comp[2] && comp[4] != comp[0] && comp[4] != comp[2],
          "Different components must have different numbers");

  dfs_h::n = 4;
  dfs_h::g = {{1, 3}, {2}, {}, {}};
  dfs_h::tin.assign(4, -1);
  dfs_h::tout.assign(4, 0);
  dfs_h::T = 0;
  dfs_h::dfs(0);
  Require(dfs_h::anc(0, 2) && dfs_h::anc(1, 2) && dfs_h::anc(0, 3) && dfs_h::anc(2, 2),
          "Ancestor relation missed a real ancestor");
  Require(!dfs_h::anc(1, 3) && !dfs_h::anc(2, 1) && !dfs_h::anc(3, 0),
          "Ancestor relation reported a false ancestor");

  for (int trial = 0; trial < 1000; ++trial) {
    int n = Rand(1, 8);
    Graph g(n);
    DSU dsu(n);
    for (int i = Rand(0, 12); i > 0; --i) {
      int a = Rand(0, n - 1), b = Rand(0, n - 1);
      g[a].push_back(b);
      g[b].push_back(a);
      dsu.u(a, b);
    }
    dfs_h::n = n;
    dfs_h::g = g;
    int count = dfs_h::components(comp);
    int expected = 0;
    for (int v = 0; v < n; ++v) expected += dsu.f(v) == v;
    Require(count == expected, "Component count differs from DSU");
    for (int a = 0; a < n; ++a)
      for (int b = 0; b < n; ++b)
        Require((comp[a] == comp[b]) == (dsu.f(a) == dsu.f(b)), "Component labels differ from DSU");

    // Цикл в ориентированном графе: вершина на цикле <=> reach[v][v].
    Graph directed(n);
    for (int i = Rand(0, 10); i > 0; --i) directed[Rand(0, n - 1)].push_back(Rand(0, n - 1));
    dfs_h::g = directed;
    std::vector<int> color(n, 0);
    bool found = false;
    for (int v = 0; v < n && !found; ++v) {
      if (color[v] == 0) found = dfs_h::cyc(v, color);
    }
    bool expected_cycle = false;
    auto reach = Closure(directed);
    for (int v = 0; v < n; ++v) expected_cycle |= reach[v][v];
    Require(found == expected_cycle, "Cycle detection differs from closure");
  }
}

void TestBfs() {
  Graph g = {{1, 2}, {3}, {3}, {}, {}};
  std::vector<int> par;
  auto d = bfs_h::bfs({0}, g, par);
  Require(d == std::vector<int>({0, 1, 1, 2, -1}), "BFS distances differ");
  Require(bfs_h::path(3, d, par) == std::vector<int>({0, 1, 3}), "Restored path differs");
  Require(bfs_h::path(4, d, par).empty(), "Unreachable vertex must give an empty path");
  Require(bfs_h::path(0, d, par) == std::vector<int>({0}), "Path to the source is the source");

  for (int trial = 0; trial < 1000; ++trial) {
    int n = Rand(1, 8);
    Edges edges = RandomEdges(n, Rand(0, 20), 1, 1);
    Graph ug(n);
    std::vector<std::vector<char>> adj(n, std::vector<char>(n, 0));
    for (auto [a, b, w] : edges) ug[a].push_back(b), adj[a][b] = 1;
    auto wg = ToWeighted(n, edges);

    std::vector<int> sources = {0};
    if (Rand(0, 1)) sources.push_back(Rand(0, n - 1));
    auto dist = bfs_h::bfs(sources, ug, par);
    std::vector<ll> expected(n, INF);
    for (int s : sources) {
      auto from_s = dijkstra_h::dijkstra(s, wg);
      for (int v = 0; v < n; ++v) expected[v] = std::min(expected[v], from_s[v]);
    }
    for (int v = 0; v < n; ++v) {
      Require((expected[v] >= INF ? -1 : static_cast<int>(expected[v])) == dist[v],
              "BFS distance differs from Dijkstra");
      auto p = bfs_h::path(v, dist, par);
      if (dist[v] < 0) {
        Require(p.empty(), "Unreachable vertex must give an empty path");
        continue;
      }
      Require(static_cast<int>(p.size()) == dist[v] + 1 && p.back() == v, "Path length differs");
      Require(std::count(sources.begin(), sources.end(), p.front()) > 0, "Path must start at a source");
      for (size_t i = 0; i + 1 < p.size(); ++i) Require(adj[p[i]][p[i + 1]], "Path uses a missing edge");
    }

    // 0-1 BFS против Дейкстры.
    std::vector<std::vector<std::pair<int, int>>> g01(n);
    WeightedGraph w01(n);
    for (auto [a, b, w] : RandomEdges(n, Rand(0, 20), 0, 1)) {
      g01[a].push_back({static_cast<int>(b), static_cast<int>(w)});
      w01[a].push_back({static_cast<int>(b), w});
    }
    auto z = bfs_h::bfs01(0, g01);
    auto expected01 = dijkstra_h::dijkstra(0, w01);
    for (int v = 0; v < n; ++v)
      Require((expected01[v] >= INF ? INT_MAX : static_cast<int>(expected01[v])) == z[v],
              "0-1 BFS differs from Dijkstra");
  }
}

void TestDijkstra() {
  WeightedGraph wide = {{{1, 3000000000LL}}, {{2, 4000000000LL}}, {}};
  Require(dijkstra_h::dijkstra(0, wide) == std::vector<ll>({0, 3000000000LL, 7000000000LL}),
          "Dijkstra must accept 64-bit weights and distances");
  WeightedGraph near_inf = {{{1, INF - 1}}, {{2, INF - 1}}, {}};
  Require(dijkstra_h::dijkstra(0, near_inf)[2] == INF,
          "A sum beyond INF must not overflow into a shorter distance");
  WeightedGraph g = {{{1, 4}, {2, 1}}, {{3, 1}}, {{1, 2}, {3, 5}}, {}, {}};
  std::vector<int> par;
  auto d = dijkstra_h::dijkstra(0, g, &par);
  Require(d[3] == 4 && d[1] == 3 && d[4] == INF, "Distances differ");
  Require(par[3] == 1 && par[1] == 2 && par[2] == 0 && par[0] == -1, "Parents differ");

  for (int trial = 0; trial < 1000; ++trial) {
    int n = Rand(1, 8);
    Edges edges = RandomEdges(n, Rand(0, 20), 0, 10);
    auto wg = ToWeighted(n, edges);
    auto matrix = BuildMatrix(n, edges);
    auto dist = dijkstra_h::dijkstra(0, wg, &par);
    std::vector<ll> bellman;
    Require(!bellman_h::bellman(n, 0, edges, bellman), "Non-negative graph has no negative cycle");
    floyd_h::floyd(matrix);
    auto direct = BuildMatrix(n, edges);
    for (int v = 0; v < n; ++v) {
      Require(dist[v] == matrix[0][v], "Dijkstra differs from Floyd");
      Require(dist[v] == bellman[v], "Dijkstra differs from Bellman-Ford");
      if (v != 0 && dist[v] < INF) {
        Require(dist[par[v]] + direct[par[v]][v] == dist[v], "Parent edge is not tight");
      }
    }
    Require(par[0] == -1, "The source has no parent");
  }
}

void TestBellmanFord() {
  std::vector<ll> d;
  Edges positive = {{0, 1, 4}, {1, 2, -2}, {0, 2, 5}};
  Require(!bellman_h::bellman(3, 0, positive, d) && d[2] == 2, "Negative edges without cycles failed");
  Edges cycle = {{0, 1, 1}, {1, 2, -3}, {2, 1, 1}};
  Require(bellman_h::bellman(3, 0, cycle, d), "Reachable negative cycle must be reported");
  Edges unreachable = {{1, 2, -1}, {2, 1, -1}};
  Require(!bellman_h::bellman(3, 0, unreachable, d) && d[1] == INF && d[0] == 0,
          "Unreachable negative cycle must be ignored");
  Edges self_loop = {{0, 0, -1}};
  Require(bellman_h::bellman(1, 0, self_loop, d), "Negative self loop is a negative cycle");
  Edges empty;
  Require(!bellman_h::bellman(1, 0, empty, d) && d[0] == 0, "Graph without edges failed");

  for (int trial = 0; trial < 2000; ++trial) {
    int n = Rand(1, 7);
    Edges edges = RandomEdges(n, Rand(0, 20), -3, 10);
    auto matrix = BuildMatrix(n, edges);
    floyd_h::floyd(matrix);
    bool expected_cycle = false;
    for (int t = 0; t < n; ++t) expected_cycle |= matrix[0][t] < INF && matrix[t][t] < 0;
    bool found = bellman_h::bellman(n, 0, edges, d);
    Require(found == expected_cycle, "Negative cycle detection differs from Floyd");
    if (!found) {
      for (int v = 0; v < n; ++v) Require(d[v] == matrix[0][v], "Distances differ from Floyd");
    }
  }
}

void TestFloyd() {
  Matrix d = {{0, 3, INF}, {INF, 0, 2}, {1, INF, 0}};
  floyd_h::floyd(d);
  Require(d[0][2] == 5 && d[2][1] == 4 && d[1][0] == 3, "Distances differ");

  Matrix cycle = {{0, 1}, {-3, 0}};
  floyd_h::floyd(cycle);
  Require(cycle[0][0] < 0 && cycle[1][1] < 0, "Negative cycle must show on the diagonal");
  Require(cycle[0][1] >= -INF && cycle[1][0] >= -INF, "Values must not overflow");

  Matrix many(40, std::vector<ll>(40, -1000000000LL));  // полный граф: значения растут экспоненциально
  for (int i = 0; i < 40; ++i) many[i][i] = 0;
  floyd_h::floyd(many);
  for (const auto& row : many)
    for (ll x : row) Require(x < 0 && x >= -INF, "Values on negative cycles must stay clamped to -INF");

  for (int trial = 0; trial < 500; ++trial) {
    int n = Rand(1, 7);
    Edges edges = RandomEdges(n, Rand(0, 20), 0, 10);
    auto matrix = BuildMatrix(n, edges);
    floyd_h::floyd(matrix);
    auto weighted = ToWeighted(n, edges);
    for (int s = 0; s < n; ++s) {
      auto expected = dijkstra_h::dijkstra(s, weighted);
      for (int v = 0; v < n; ++v) Require(matrix[s][v] == expected[v], "Floyd differs from Dijkstra");
    }
  }
}

void TestToposort() {
  Graph empty;
  Require(toposort_h::toposort(empty).empty(), "Empty graph has an empty order");
  Graph single = {{}};
  Require(toposort_h::toposort(single) == std::vector<int>({0}), "Single vertex order differs");
  Graph chain = {{1}, {2}, {}};
  Require(toposort_h::toposort(chain) == std::vector<int>({0, 1, 2}), "Chain order differs");
  Graph cycle = {{1}, {2}, {0}};
  Require(toposort_h::toposort(cycle).empty(), "Cycle must give an empty order");
  Graph self_loop = {{0}};
  Require(toposort_h::toposort(self_loop).empty(), "Self loop is a cycle");

  for (int trial = 0; trial < 1000; ++trial) {
    int n = Rand(1, 8);
    Graph g(n);
    for (int i = Rand(0, 14); i > 0; --i) g[Rand(0, n - 1)].push_back(Rand(0, n - 1));
    auto reach = Closure(g);
    bool has_cycle = false;
    for (int v = 0; v < n; ++v) has_cycle |= reach[v][v];
    auto order = toposort_h::toposort(g);
    if (has_cycle) {
      Require(order.empty(), "Graph with a cycle must give an empty order");
      continue;
    }
    Require(static_cast<int>(order.size()) == n, "Order must contain every vertex");
    std::vector<int> position(n, -1);
    for (int i = 0; i < n; ++i) position[order[i]] = i;
    for (int v = 0; v < n; ++v) {
      Require(position[v] >= 0, "Order must be a permutation");
      for (int to : g[v]) Require(position[v] < position[to], "Edge goes backwards in the order");
    }
  }
}

void TestDsu() {
  DSU d(4);
  Require(d.u(0, 1) && !d.u(1, 0) && !d.u(2, 2), "Union result differs");
  Require(d.sz[d.f(0)] == 2 && d.f(0) == d.f(1) && d.f(2) != d.f(0), "Component state differs");

  for (int trial = 0; trial < 300; ++trial) {
    int n = Rand(1, 30);
    DSU dsu(n);
    std::vector<int> label(n);
    std::iota(label.begin(), label.end(), 0);
    for (int step = 0; step < 100; ++step) {
      int a = Rand(0, n - 1), b = Rand(0, n - 1);
      bool merged = label[a] != label[b];
      if (merged) {
        int from = label[b], to = label[a];
        for (int& x : label) if (x == from) x = to;
      }
      Require(dsu.u(a, b) == merged, "Union result differs from relabeling");
      int c = Rand(0, n - 1), e = Rand(0, n - 1);
      Require((dsu.f(c) == dsu.f(e)) == (label[c] == label[e]), "Connectivity differs");
      Require(dsu.sz[dsu.f(c)] == std::count(label.begin(), label.end(), label[c]), "Size differs");
    }
  }
}

void TestKruskal() {
  auto large = kruskal(4, {{1000000000LL, 0, 1}, {1000000000LL, 1, 2},
                           {1000000000LL, 2, 3}});
  Require(large == std::pair<ll, int>{3000000000LL, 3}, "MST sum must not overflow int");
  auto signed_sum = kruskal(4, {{-6000000000000000000LL, 0, 1},
                               {-6000000000000000000LL, 1, 2},
                               {6000000000000000000LL, 2, 3}});
  Require(signed_sum == std::pair<ll, int>{-6000000000000000000LL, 3},
          "MST prefix sums may exceed ll even when the final sum fits");
  auto wide = kruskal(2, {{5000000000LL, 0, 1}});
  Require(wide == std::pair<ll, int>{5000000000LL, 1}, "MST must accept 64-bit edge weights");
  auto [weight, count] = kruskal(3, {{1, 0, 1}, {2, 1, 2}, {3, 0, 2}});
  Require(weight == 3 && count == 2, "Spanning tree differs");
  auto [forest_weight, forest_count] = kruskal(4, {{1, 0, 1}, {2, 1, 2}, {3, 0, 2}});
  Require(forest_weight == 3 && forest_count == 2, "Spanning forest of a disconnected graph differs");
  auto [none_weight, none_count] = kruskal(3, {});
  Require(none_weight == 0 && none_count == 0, "Graph without edges differs");
  auto [loop_weight, loop_count] = kruskal(2, {{-5, 0, 0}, {7, 0, 1}, {9, 0, 1}});
  Require(loop_weight == 7 && loop_count == 1, "Loops and parallel edges differ");

  for (int trial = 0; trial < 1000; ++trial) {
    int n = Rand(1, 6), m = Rand(0, 9);
    std::vector<std::array<ll, 3>> edges;  // {w, a, b}
    for (int i = 0; i < m; ++i) edges.push_back({Rand(-5, 10), Rand(0, n - 1), Rand(0, n - 1)});
    DSU all_edges(n);
    int components = n;
    for (auto [w, a, b] : edges) components -= all_edges.u(a, b);

    ll best = INF;  // минимум по лесам из n - components рёбер
    for (int mask = 0; mask < (1 << m); ++mask) {
      if (__builtin_popcount(mask) != n - components) continue;
      DSU dsu(n);
      ll sum = 0;
      bool forest = true;
      for (int i = 0; i < m; ++i) {
        if (mask >> i & 1) {
          forest &= dsu.u(edges[i][1], edges[i][2]);
          sum += edges[i][0];
        }
      }
      if (forest) best = std::min(best, sum);
    }
    auto [w, c] = kruskal(n, edges);
    Require(c == n - components && w == best, "Kruskal differs from brute force");
  }
}

int CountComponents(int n, const std::vector<std::array<int, 2>>& edges, int skip_edge, int skip_vertex) {
  DSU dsu(n);
  for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
    auto [a, b] = edges[i];
    if (i != skip_edge && a != skip_vertex && b != skip_vertex) dsu.u(a, b);
  }
  int count = 0;
  for (int v = 0; v < n; ++v) count += v != skip_vertex && dsu.f(v) == v;
  return count;
}

void TestBridges() {
  bridges_h::n = 4;
  bridges_h::g.assign(4, {});
  std::vector<std::array<int, 2>> triangle_with_tail = {{0, 1}, {1, 2}, {2, 0}, {2, 3}};
  for (int id = 0; id < 4; ++id) {
    auto [a, b] = triangle_with_tail[id];
    bridges_h::g[a].push_back({b, id});
    bridges_h::g[b].push_back({a, id});
  }
  bridges_h::run();
  Require(bridges_h::bridges == std::vector<int>({3}), "Bridge set differs");
  Require(!bridges_h::cut[0] && !bridges_h::cut[1] && bridges_h::cut[2] && !bridges_h::cut[3],
          "Articulation points differ");

  for (int trial = 0; trial < 2000; ++trial) {
    int n = Rand(1, 8), m = Rand(0, 11);
    std::vector<std::array<int, 2>> edges;
    bridges_h::n = n;
    bridges_h::g.assign(n, {});
    for (int id = 0; id < m; ++id) {  // кратные рёбра и петли разрешены
      int a = Rand(0, n - 1), b = Rand(0, n - 1);
      edges.push_back({a, b});
      bridges_h::g[a].push_back({b, id});
      bridges_h::g[b].push_back({a, id});
    }
    bridges_h::run();

    std::set<int> found(bridges_h::bridges.begin(), bridges_h::bridges.end());
    Require(found.size() == bridges_h::bridges.size(), "A bridge was reported twice");
    int base = CountComponents(n, edges, -1, -1);
    for (int id = 0; id < m; ++id) {
      Require((CountComponents(n, edges, id, -1) > base) == (found.count(id) > 0), "Bridge differs from brute force");
    }
    for (int v = 0; v < n; ++v) {
      bool alone = true;  // у v нет рёбер к другим вершинам
      for (auto [a, b] : edges) alone &= !((a == v) != (b == v));
      Require((CountComponents(n, edges, -1, v) > base - (alone ? 1 : 0)) == static_cast<bool>(bridges_h::cut[v]),
              "Articulation point differs from brute force");
    }
  }
}

void TestLca() {
  lca_h::n = 2;
  lca_h::g = {{1}, {0}};
  lca_h::build(0);
  Require(lca_h::kth(1, 1 << lca_h::LOG) == 0 && lca_h::kth(1, INT_MAX) == 0,
          "Large k must return the root, not ignore its high bits");
  lca_h::build(1);
  Require(lca_h::kth(0, INT_MAX) == 1, "Large k must respect a nonzero root");
  lca_h::n = 1;
  lca_h::g = {{}};
  lca_h::build(0);
  Require(lca_h::lca(0, 0) == 0 && lca_h::dist(0, 0) == 0 && lca_h::kth(0, 5) == 0, "Single vertex differs");

  for (int trial = 0; trial < 300; ++trial) {
    int n = Rand(1, 40);
    std::vector<int> parent(n, -1);
    lca_h::n = n;
    lca_h::g.assign(n, {});
    for (int v = 1; v < n; ++v) {
      parent[v] = Rand(0, v - 1);
      lca_h::g[parent[v]].push_back(v);
      lca_h::g[v].push_back(parent[v]);
    }
    lca_h::build(0);
    auto ancestors = [&](int v) {
      std::vector<int> chain;
      for (; v != -1; v = parent[v]) chain.push_back(v);
      return chain;
    };
    for (int query = 0; query < 30; ++query) {
      int a = Rand(0, n - 1), b = Rand(0, n - 1);
      auto up_a = ancestors(a), up_b = ancestors(b);
      int expected = -1;
      for (int x : up_a) {
        if (std::find(up_b.begin(), up_b.end(), x) != up_b.end()) {
          expected = x;
          break;
        }
      }
      Require(lca_h::lca(a, b) == expected, "LCA differs from the naive answer");
      int depth = (std::find(up_a.begin(), up_a.end(), expected) - up_a.begin()) +
                  (std::find(up_b.begin(), up_b.end(), expected) - up_b.begin());
      Require(lca_h::dist(a, b) == depth, "Distance differs from the naive answer");
      int k = Rand(0, 45);
      Require(lca_h::kth(a, k) == (k < static_cast<int>(up_a.size()) ? up_a[k] : 0),
              "k-th ancestor differs from the naive answer");
    }
  }

  int n = 5000;  // цепочка: проверяем старшие биты в kth
  lca_h::n = n;
  lca_h::g.assign(n, {});
  for (int v = 1; v < n; ++v) {
    lca_h::g[v - 1].push_back(v);
    lca_h::g[v].push_back(v - 1);
  }
  lca_h::build(0);
  Require(lca_h::kth(n - 1, 4000) == n - 1 - 4000, "k-th ancestor on a long chain differs");
  Require(lca_h::lca(n - 1, 17) == 17 && lca_h::dist(n - 1, 0) == n - 1, "LCA on a long chain differs");
}

void TestTernary() {
  auto parabola = [](double x) { return (x - 3.5) * (x - 3.5) + 2; };
  Require(std::abs(ternMin(-10, 10, parabola) - 3.5) < 1e-6, "Minimum of a parabola differs");
  Require(std::abs(ternMin(0, 10, [](double x) { return x; })) < 1e-6, "Minimum at the left end differs");
  Require(std::abs(ternMin(0, 10, [](double x) { return -x; }) - 10) < 1e-6, "Minimum at the right end differs");
  // Максимум g(x) = 5 - (x - 2)^2 ищется как минимум -g(x).
  auto g = [](double x) { return 5 - (x - 2) * (x - 2); };
  Require(std::abs(ternMin(-10, 10, [&](double x) { return -g(x); }) - 2) < 1e-6, "Maximum via negation differs");

  Require(ternMinInt(5, 5, [](int) { return 0; }) == 5, "Single point differs");
  for (int trial = 0; trial < 3000; ++trial) {
    int n = Rand(1, 30), peak = Rand(0, n - 1);
    std::vector<int> a(n);  // строго убывает до peak, затем строго возрастает (на дне возможна пара равных)
    for (int i = peak; i >= 0; --i) a[i] = i == peak ? 0 : a[i + 1] + 1 + Rand(0, 2);
    for (int i = peak + 1; i < n; ++i) a[i] = i == peak + 1 ? (Rand(0, 1) ? 0 : 1 + Rand(0, 2)) : a[i - 1] + 1 + Rand(0, 2);
    int best = *std::min_element(a.begin(), a.end());
    int index = ternMinInt(0, n - 1, [&](int i) { return a[i]; });
    Require(0 <= index && index < n && a[index] == best, "Integer minimum differs from scan");
  }
}

}  // namespace

signed main() {
  try {
    TestDfs();
    TestBfs();
    TestDijkstra();
    TestBellmanFord();
    TestFloyd();
    TestToposort();
    TestDsu();
    TestKruskal();
    TestBridges();
    TestLca();
    TestTernary();
  } catch (const std::exception& error) {
    std::cerr << "Graphs test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "Graphs tests passed\n";
}