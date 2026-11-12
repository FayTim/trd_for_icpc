#pragma once

#include <cstddef>
#include <vector>

struct SccResult {
  int count;
  std::vector<int> component;
};

inline SccResult FindScc(const std::vector<std::vector<int>>& graph) {
  const int vertices = static_cast<int>(graph.size());

  std::vector<std::vector<int>> reverse_graph(vertices);
  for (int v = 0; v < vertices; ++v) {
    for (const int to : graph[v]) {
      reverse_graph[to].push_back(v);
    }
  }

  std::vector<char> visited(vertices, false);
  std::vector<std::size_t> next_edge(vertices, 0);
  std::vector<int> order;
  std::vector<int> stack;
  order.reserve(vertices);
  stack.reserve(vertices);

  for (int start = 0; start < vertices; ++start) {
    if (visited[start]) {
      continue;
    }

    visited[start] = true;
    stack.push_back(start);

    while (!stack.empty()) {
      const int v = stack.back();
      if (next_edge[v] == graph[v].size()) {
        order.push_back(v);
        stack.pop_back();
        continue;
      }

      const int to = graph[v][next_edge[v]++];
      if (!visited[to]) {
        visited[to] = true;
        stack.push_back(to);
      }
    }
  }

  SccResult result{0, std::vector<int>(vertices, -1)};
  for (auto it = order.rbegin(); it != order.rend(); ++it) {
    const int start = *it;
    if (result.component[start] != -1) {
      continue;
    }

    result.component[start] = result.count;
    stack.push_back(start);

    while (!stack.empty()) {
      const int v = stack.back();
      stack.pop_back();

      for (const int to : reverse_graph[v]) {
        if (result.component[to] == -1) {
          result.component[to] = result.count;
          stack.push_back(to);
        }
      }
    }

    ++result.count;
  }

  return result;
}
