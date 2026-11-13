#pragma once

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

namespace euler_h {

struct Walk {
  std::vector<int> vertices, edge_ids;
};

// Hierholzer; reference: KACTL EulerWalk.h (Simon Lindholm, CC0).
// Each input edge occurs once; its id is its index in edges.
// Preconditions: n >= 0, valid endpoints, n and 2*edges.size()+1 fit in int.
// start = -1 chooses a start; otherwise 0 <= start < n is required.
// nullopt means no Euler trail from the requested (or any) start.
// Time/memory O(n+m). Input is unchanged; loops and parallel edges allowed.
inline std::optional<Walk> eulerWalk(
    int n, const std::vector<std::pair<int, int>>& edges,
    bool directed, int start = -1) {
  const int m = static_cast<int>(edges.size());
  if (n == 0) return Walk{};
  if (m == 0) return Walk{{start == -1 ? 0 : start}, {}};
  std::vector<std::vector<std::pair<int, int>>> g(n);
  std::vector<int> degree(n), next(n);
  for (int id = 0; id < m; ++id) {
    const auto [a, b] = edges[id];
    g[a].push_back({b, id});
    ++degree[a];
    if (directed) --degree[b];  // outdegree - indegree
    else {
      g[b].push_back({a, id});
      ++degree[b];             // a loop contributes two to the degree
    }
  }
  int first = -1, last = -1, odd = 0;
  for (int v = 0; v < n; ++v) {
    if (directed) {
      if (degree[v] == 1 && first == -1) first = v;
      else if (degree[v] == -1 && last == -1) last = v;
      else if (degree[v] != 0) return std::nullopt;
    } else if (degree[v] % 2) {
      first = v;
      ++odd;
    }
  }
  if (directed) {
    if ((first == -1) != (last == -1)) return std::nullopt;
    if (first != -1 && start != -1 && start != first) return std::nullopt;
  } else {
    if (odd != 0 && odd != 2) return std::nullopt;
    if (odd == 2 && start != -1 && degree[start] % 2 == 0)
      return std::nullopt;
  }
  if (start == -1) start = first != -1 ? first : edges[0].first;
  std::vector<char> used(m);
  std::vector<std::pair<int, int>> stack{{start, -1}};
  Walk answer;
  while (!stack.empty()) {
    const auto [v, incoming] = stack.back();
    if (next[v] == static_cast<int>(g[v].size())) {
      answer.vertices.push_back(v);
      if (incoming != -1) answer.edge_ids.push_back(incoming);
      stack.pop_back();
    } else {
      const auto [to, id] = g[v][next[v]++];
      if (!used[id]) {
        used[id] = true;
        stack.push_back({to, id});
      }
    }
  }
  // Degree conditions alone do not reject disconnected edge components.
  if (static_cast<int>(answer.edge_ids.size()) != m) return std::nullopt;
  std::reverse(answer.vertices.begin(), answer.vertices.end());
  std::reverse(answer.edge_ids.begin(), answer.edge_ids.end());
  return answer;
}

}  // namespace euler_h
