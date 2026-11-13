#include "range_statistics.h"
#include "rollback_mo.h"

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

int main() {
  std::vector<int> a{0, 1, 0, 2, 1};
  const std::vector<Update> changes{{1, 1, 0}, {4, 1, 2}};
  ChangingFrequencies changing(a, 3, changes);
  const std::vector<mo::TimedQuery> queries{{0, 3, 0}, {0, 3, 1}, {0, 5, 2}};
  for (const auto& s : mo::SolveWithUpdates(5, queries, 2, changing))
    std::cout << s.distinct << ' ';  // 2 1 2
  std::cout << '\n';

  const std::vector<int> colors{0, 1, 0, 2, 1};
  for (int f : mo::RollbackModeFrequency(colors, {{0, 5}, {1, 4}, {2, 2}}))
    std::cout << f << ' ';  // 2 1 0
  std::cout << '\n';
}
