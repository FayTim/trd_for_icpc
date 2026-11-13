#include "euler.h"

#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

using Edges = std::vector<std::pair<int, int>>;

void Require(bool ok, const char* message) {
  if (!ok) throw std::runtime_error(message);
}

// Independent search through subsets of used edges, for small graphs only.
bool BruteExists(int n, const Edges& edges, bool directed, int start) {
  if (n == 0) return edges.empty();
  const int masks = 1 << edges.size();
  std::vector<int> memo(n * masks, -1);
  auto visit = [&](auto&& self, int v, int mask) -> bool {
    if (mask == masks - 1) return true;
    int& answer = memo[mask * n + v];
    if (answer != -1) return answer;
    answer = 0;
    for (int id = 0; id < static_cast<int>(edges.size()); ++id) {
      if (mask & (1 << id)) continue;
      const auto [a, b] = edges[id];
      const int to = a == v ? b : (!directed && b == v ? a : -1);
      if (to != -1 && self(self, to, mask | (1 << id))) {
        answer = 1;
        break;
      }
    }
    return answer;
  };
  if (start != -1) return visit(visit, start, 0);
  for (int v = 0; v < n; ++v)
    if (visit(visit, v, 0)) return true;
  return false;
}

void Validate(int n, const Edges& edges, bool directed, int start,
              const euler_h::Walk& walk) {
  Require(walk.edge_ids.size() == edges.size(), "Not all edges used");
  if (n == 0) {
    Require(walk.vertices.empty(), "Empty graph must have no vertices");
    return;
  }
  Require(walk.vertices.size() == edges.size() + 1,
          "Walk must have one more vertex than edges");
  if (start != -1)
    Require(walk.vertices.front() == start, "Requested start ignored");
  for (int v : walk.vertices)
    Require(0 <= v && v < n, "Vertex out of range");
  std::vector<char> used(edges.size());
  for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
    const int id = walk.edge_ids[i];
    Require(0 <= id && id < static_cast<int>(edges.size()), "Edge id out of range");
    Require(!used[id], "Edge used more than once");
    used[id] = true;
    const auto [a, b] = edges[id];
    const int from = walk.vertices[i], to = walk.vertices[i + 1];
    Require((a == from && b == to) || (!directed && b == from && a == to),
            "Edge id does not match consecutive vertices or direction");
  }
}

void Check(int n, const Edges& edges) {
  const auto original = edges;
  for (bool directed : {false, true}) {
    for (int start = -1; start < n; ++start) {
      const bool expected = BruteExists(n, edges, directed, start);
      for (int repeat = 0; repeat < 2; ++repeat) {
        auto result = euler_h::eulerWalk(n, edges, directed, start);
        Require(result.has_value() == expected, "Existence differs from brute force");
        Require(edges == original, "Input edges changed");
        if (result) Validate(n, edges, directed, start, *result);
      }
    }
  }
}

void TestSmallGraphs() {
  Check(0, {});
  Check(1, {});
  Check(4, {});
  Check(1, {{0, 0}, {0, 0}, {0, 0}});
  Check(4, {{2, 3}});  // Vertex 0 is isolated; auto start must skip it.
  Check(3, {{0, 1}, {1, 2}});  // Directed start vs both undirected odd ends.
  Check(3, {{0, 1}, {0, 2}});  // Impossible directed degrees.
  Check(4, {{0, 1}, {0, 2}, {0, 3}});  // Four odd degrees.
  Check(4, {{0, 1}, {1, 0}, {2, 3}, {3, 2}});  // Balanced, disconnected.
  Check(3, {{0, 0}, {2, 2}});  // Disconnected loops.
  Check(2, {{0, 1}, {0, 1}, {1, 0}, {1, 1}, {0, 0}});
  Check(4, {{1, 2}, {2, 3}, {3, 1}});  // Cycle with isolated vertex 0.

  // Exhaust all ordered edge lists on two vertices through length five.
  for (int m = 0, count = 1; m <= 5; ++m, count *= 4) {
    for (int code = 0; code < count; ++code) {
      Edges edges;
      int value = code;
      for (int i = 0; i < m; ++i, value /= 4)
        edges.emplace_back((value % 4) / 2, value % 2);
      Check(2, edges);
    }
  }
  std::mt19937 rng(0xe0132026);
  for (int trial = 0; trial < 800; ++trial) {
    const int n = 1 + rng() % 5, m = rng() % 9;
    Edges edges;
    for (int i = 0; i < m; ++i) edges.emplace_back(rng() % n, rng() % n);
    Check(n, edges);
  }
}

void TestLongChain() {
  constexpr int n = 200000;
  Edges edges;
  for (int v = 1; v < n; ++v) edges.emplace_back(v - 1, v);
  for (bool directed : {false, true}) {
    auto result = euler_h::eulerWalk(n, edges, directed);
    Require(result.has_value(), "Long chain must have a walk");
    Validate(n, edges, directed, -1, *result);
    Require(!euler_h::eulerWalk(n, edges, directed, n / 2),
            "Chain cannot start in its middle");
  }
  auto reverse = euler_h::eulerWalk(n, edges, false, n - 1);
  Require(reverse.has_value(), "Undirected chain must work from either end");
  Validate(n, edges, false, n - 1, *reverse);
  Require(!euler_h::eulerWalk(n, edges, true, n - 1),
          "Directed chain cannot start at the sink");
}

}  // namespace

int main() {
  try {
    TestSmallGraphs();
    TestLongChain();
    std::cout << "Euler walks: brute-force and long-chain checks passed\n";
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
