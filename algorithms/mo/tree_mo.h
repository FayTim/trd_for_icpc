#pragma once

#include "mo.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace mo {

// A connected, undirected tree. Vertices and path endpoints are 0-based.
// Iterative DFS avoids recursion depth limits on long chains.
class TreeMo {
 public:
  explicit TreeMo(const std::vector<std::vector<int>>& graph, int root = 0)
      : root_(root), tin_(graph.size()), tout_(graph.size()) {
    const int n = static_cast<int>(graph.size());
    if (n == 0) return;  // Only an empty list of queries is valid.
    int levels = 1;
    while ((1LL << levels) <= n) ++levels;
    up_.assign(levels, std::vector<int>(n, root));
    std::vector<int> next(n), stack{root};
    euler_.reserve(2 * graph.size());
    tin_[root] = 0;
    euler_.push_back(root);
    while (!stack.empty()) {
      const int v = stack.back();
      if (next[v] == static_cast<int>(graph[v].size())) {
        tout_[v] = static_cast<int>(euler_.size());
        euler_.push_back(v);
        stack.pop_back();
        continue;
      }
      const int w = graph[v][next[v]++];
      if (w == up_[0][v]) continue;
      up_[0][w] = v;
      for (int j = 1; j < levels; ++j)
        up_[j][w] = up_[j - 1][up_[j - 1][w]];
      tin_[w] = static_cast<int>(euler_.size());
      euler_.push_back(w);
      stack.push_back(w);
    }
  }

  int Lca(int u, int v) const {
    if (IsAncestor(u, v)) return u;
    if (IsAncestor(v, u)) return v;
    for (int j = static_cast<int>(up_.size()) - 1; j >= 0; --j)
      if (!IsAncestor(up_[j][u], v)) u = up_[j][u];
    return up_[0][u];
  }

  // State starts empty; AddVertex(v), RemoveVertex(v), Answer(query_id).
  // The statistic must depend on the multiset of vertices, not path order.
  // edge_values=true: vertex v represents edge (parent[v], v); root is ignored.
  // State is left at the final internal Euler interval; reset it before reuse.
  template <class State>
  auto SolvePaths(const std::vector<std::pair<int, int>>& paths, State& state,
                  bool edge_values = false, Order order = Order::Snake) const {
    std::vector<Query> queries;
    std::vector<int> extra;
    queries.reserve(paths.size());
    extra.reserve(paths.size());
    for (auto [u, v] : paths) {
      if (tin_[u] > tin_[v]) std::swap(u, v);
      const int lca = Lca(u, v);
      if (lca == u) {
        queries.push_back({tin_[u] + (edge_values ? 1 : 0), tin_[v] + 1});
        extra.push_back(-1);
      } else {
        queries.push_back({tout_[u], tin_[v] + 1});
        extra.push_back(edge_values ? -1 : lca);
      }
    }
    struct Adapter {
      const std::vector<int>& euler;
      const std::vector<int>& extra;
      State& state;
      std::vector<char> active;
      int ignored;

      void Toggle(int v) {
        if (v == ignored) return;
        if (active[v]) state.RemoveVertex(v);
        else state.AddVertex(v);
        active[v] ^= 1;
      }
      void AddLeft(int i) { Toggle(euler[i]); }
      void AddRight(int i) { Toggle(euler[i]); }
      void RemoveLeft(int i) { Toggle(euler[i]); }
      void RemoveRight(int i) { Toggle(euler[i]); }
      auto Answer(int id) {
        if (extra[id] != -1) Toggle(extra[id]);
        auto answer = state.Answer(id);
        if (extra[id] != -1) Toggle(extra[id]);
        return answer;
      }
    } adapter{euler_, extra, state, std::vector<char>(tin_.size()),
              edge_values ? root_ : -1};
    return mo::Solve(static_cast<int>(euler_.size()), queries, adapter, order);
  }

 private:
  bool IsAncestor(int u, int v) const {
    return tin_[u] <= tin_[v] && tout_[v] <= tout_[u];
  }

  int root_;
  std::vector<int> tin_, tout_, euler_;
  std::vector<std::vector<int>> up_;
};

}  // namespace mo
