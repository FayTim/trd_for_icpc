#include "range_statistics.h"
#include "rollback_mo.h"
#include "tree_mo.h"

#include <iostream>
#include <vector>

// Values here are already compressed, including all future values.
struct Update { int position, before, after; };

struct ChangingFrequencies : mo::FrequencyState {
  std::vector<int>& current;
  const std::vector<Update>& updates;
  ChangingFrequencies(std::vector<int>& a, int sigma,
                      const std::vector<Update>& changes)
      : FrequencyState(a, sigma), current(a), updates(changes) {}
  void Apply(int id, bool forward, int l, int r) {
    const auto [p, before, after] = updates[id];
    const bool inside = l <= p && p < r;
    if (inside) RemoveRight(p);  // Frequency updates ignore position order.
    current[p] = forward ? after : before;
    if (inside) AddRight(p);
  }
};

struct TreeFrequencies : mo::FrequencyState {
  using FrequencyState::FrequencyState;
  void AddVertex(int v) { AddRight(v); }
  void RemoveVertex(int v) { RemoveRight(v); }
};

int main() {
  std::vector<int> a{0, 1, 0, 2, 1};
  const std::vector<Update> changes{{1, 1, 0}, {4, 1, 2}};
  ChangingFrequencies changing(a, 3, changes);
  const std::vector<mo::TimedQuery> queries{{0, 3, 0}, {0, 3, 1}, {0, 5, 2}};
  for (const auto& s : mo::SolveWithUpdates(5, queries, 2, changing))
    std::cout << s.distinct << ' ';  // 2 1 2
  std::cout << '\n';

  const std::vector<std::vector<int>> graph{{1, 2}, {0, 3}, {0, 4}, {1}, {2}};
  const std::vector<int> colors{0, 1, 0, 2, 1};
  mo::TreeMo tree(graph);
  TreeFrequencies nodes(colors, 3);
  const std::vector<std::pair<int, int>> paths{{3, 4}, {0, 2}, {1, 1}};
  for (const auto& s : tree.SolvePaths(paths, nodes))
    std::cout << s.distinct << ' ';  // 3 1 1
  std::cout << '\n';

  // colors[v] represents edge (parent[v], v); root is ignored.
  TreeFrequencies edges(colors, 3);
  for (const auto& s : tree.SolvePaths(paths, edges, true))
    std::cout << s.distinct << ' ';  // 3 1 0
  std::cout << '\n';

  for (int f : mo::RollbackModeFrequency(colors, {{0, 5}, {1, 4}, {2, 2}}))
    std::cout << f << ' ';  // 2 1 0
  std::cout << '\n';
}
