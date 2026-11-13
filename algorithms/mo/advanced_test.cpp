#include "rollback_mo.h"
#include "tree_mo.h"

#include <climits>
#include <deque>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>

namespace {

void Check(bool ok, const char* message) {
  if (!ok) throw std::runtime_error(message);
}

struct OrderedRollback {
  std::deque<int> current;
  std::vector<bool> from_left;

  void AddLeft(int i) {
    Check(current.empty() || current.front() == i + 1, "Bad prepend");
    current.push_front(i);
    from_left.push_back(true);
  }
  void AddRight(int i) {
    Check(current.empty() || current.back() + 1 == i, "Bad append");
    current.push_back(i);
    from_left.push_back(false);
  }
  std::size_t Snapshot() const { return from_left.size(); }
  void Rollback(std::size_t saved) {
    Check(saved <= from_left.size(), "Cannot roll forward");
    while (from_left.size() > saved) {
      if (from_left.back()) current.pop_front();
      else current.pop_back();
      from_left.pop_back();
    }
  }
  std::vector<int> Answer(int) const {
    return {current.begin(), current.end()};
  }
};

void TestRollback() {
  std::mt19937 rng(0x72a41);
  Check(mo::RollbackModeFrequency({}, {}).empty(), "Empty query list");
  Check(mo::RollbackModeFrequency({}, {{0, 0}}) == std::vector<int>{0},
        "Empty array");
  Check(mo::RollbackModeFrequency({INT_MIN, INT_MAX, INT_MIN}, {{0, 3}})
            == std::vector<int>{2}, "Extreme values compression");
  for (int trial = 0; trial < 180; ++trial) {
    const int n = 1 + rng() % 45;
    std::vector<int> a(n);
    for (int& x : a) x = static_cast<int>(rng() % 11) - 5;
    std::vector<mo::Query> queries;
    for (int l = 0; l <= n; ++l)
      for (int r = l; r <= n; ++r) queries.push_back({l, r});
    queries.push_back({0, n});
    queries.push_back({n, n});
    std::shuffle(queries.begin(), queries.end(), rng);
    std::vector<int> expected;
    std::vector<std::vector<int>> ordered;
    for (auto [l, r] : queries) {
      std::map<int, int> frequency;
      int maximum = 0;
      for (int i = l; i < r; ++i)
        maximum = std::max(maximum, ++frequency[a[i]]);
      expected.push_back(maximum);
      std::vector<int> interval(r - l);
      std::iota(interval.begin(), interval.end(), l);
      ordered.push_back(interval);
    }
    for (int block : {0, 1, 2, 7, n, n + 10}) {
      Check(mo::RollbackModeFrequency(a, queries, block) == expected,
            "Rollback mode differs from brute force");
      OrderedRollback state;
      Check(mo::SolveRollback(n, queries, state, block) == ordered,
            "Rollback interval or side mismatch");
      Check(state.current.empty() && state.from_left.empty(),
            "Rollback runner did not restore initial state");
    }
  }
}

struct VertexSet {
  std::vector<char> active;
  int ignored = -1;

  void AddVertex(int v) {
    Check(v != ignored, "Root edge must be ignored");
    Check(!active[v], "Adding active vertex");
    active[v] = true;
  }
  void RemoveVertex(int v) {
    Check(active[v], "Removing inactive vertex");
    active[v] = false;
  }
  std::vector<int> Answer(int) const {
    std::vector<int> result;
    for (int v = 0; v < static_cast<int>(active.size()); ++v)
      if (active[v]) result.push_back(v);
    return result;
  }
};

void TestTrees() {
  mo::TreeMo empty({});
  VertexSet empty_state{{}};
  Check(empty.SolvePaths({}, empty_state).empty(), "Empty tree");
  std::mt19937 rng(0x45c19);
  for (int trial = 0; trial < 200; ++trial) {
    const int n = 1 + rng() % 35, root = rng() % n;
    std::vector<std::vector<int>> graph(n);
    for (int v = 1; v < n; ++v) {
      const int p = rng() % v;
      graph[v].push_back(p);
      graph[p].push_back(v);
    }
    for (auto& neighbors : graph)
      std::shuffle(neighbors.begin(), neighbors.end(), rng);
    std::vector<int> parent(n, -1), depth(n), queue{root};
    parent[root] = root;
    for (int i = 0; i < n; ++i)
      for (int v : graph[queue[i]]) if (parent[v] == -1) {
        parent[v] = queue[i];
        depth[v] = depth[queue[i]] + 1;
        queue.push_back(v);
      }
    mo::TreeMo tree(graph, root);
    std::vector<std::pair<int, int>> paths;
    for (int u = 0; u < n; ++u)
      for (int v = 0; v < n; ++v) paths.emplace_back(u, v);
    std::shuffle(paths.begin(), paths.end(), rng);
    for (bool edges : {false, true}) {
      std::vector<std::vector<int>> expected;
      for (auto [u, v] : paths) {
        const int source = u, target = v;
        std::vector<int> vertices;
        while (u != v) {
          if (depth[u] < depth[v]) std::swap(u, v);
          vertices.push_back(u);
          u = parent[u];
        }
        Check(tree.Lca(source, target) == u, "LCA differs from brute force");
        if (!edges) vertices.push_back(u);
        std::sort(vertices.begin(), vertices.end());
        expected.push_back(vertices);
      }
      for (auto order : {mo::Order::Snake, mo::Order::Hilbert}) {
        VertexSet state{std::vector<char>(n), edges ? root : -1};
        Check(tree.SolvePaths(paths, state, edges, order) == expected,
              "Tree paths differ from brute force");
      }
    }
  }
}

void TestLongChain() {
  const int n = 200'000, root = n / 2;
  std::vector<std::vector<int>> graph(n);
  for (int i = 1; i < n; ++i) {
    graph[i - 1].push_back(i);
    graph[i].push_back(i - 1);
  }
  mo::TreeMo tree(graph, root);
  struct CountState {
    std::vector<char> active;
    int ignored, count = 0;
    void AddVertex(int v) {
      Check(v != ignored && !active[v], "Invalid chain addition");
      active[v] = true;
      ++count;
    }
    void RemoveVertex(int v) {
      Check(active[v], "Invalid chain removal");
      active[v] = false;
      --count;
    }
    int Answer(int) const { return count; }
  } nodes{std::vector<char>(n), -1}, edges{std::vector<char>(n), root};
  const std::vector<std::pair<int, int>> paths{
      {0, n - 1}, {root, root}, {root, 0}, {n - 2, n - 1}};
  Check(tree.SolvePaths(paths, nodes) == std::vector<int>{n, 1, root + 1, 2},
        "Long chain node queries");
  Check(tree.SolvePaths(paths, edges, true)
            == std::vector<int>{n - 1, 0, root, 1}, "Long chain edge queries");
}

}  // namespace

int main() {
  try {
    TestRollback();
    TestTrees();
    TestLongChain();
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
