#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

inline long long TreeIndependentSetWeight(
    const std::vector<std::vector<int>>& tree,
    const std::vector<int>& weight) {
  if (tree.empty()) return 0;

  std::vector<int> parent(tree.size(), -1);
  std::vector<int> order{0};
  for (std::size_t i = 0; i < order.size(); ++i) {
    const int v = order[i];
    for (int u : tree[v]) {
      if (u == parent[v]) continue;
      parent[u] = v;
      order.push_back(u);
    }
  }

  std::vector<long long> take(weight.begin(), weight.end());
  std::vector<long long> skip(tree.size(), 0);
  while (order.size() > 1) {
    const int v = order.back();
    order.pop_back();
    const int p = parent[v];
    take[p] += skip[v];
    skip[p] += std::max(take[v], skip[v]);
  }
  return std::max(take[0], skip[0]);
}
