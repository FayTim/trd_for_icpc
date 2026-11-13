#include "approximate_lis.h"
#include "range_statistics.h"

#include <iostream>
#include <vector>

int main() {
  const std::vector<int> a{3, 1, 3, 2, 4};
  const std::vector<mo::Query> queries{{0, 4}, {1, 5}, {2, 2}};
  auto frequency = mo::RangeFrequencies(a, queries);
  auto lis = mo::ApproximateRangeLis(a, queries);
  for (int id = 0; id < int(queries.size()); ++id) {
    std::cout << frequency[id].distinct << ' '
              << frequency[id].equal_pairs << ' '
              << lis[id] << '\n';
  }
  // k is one-based; answers are original values, not compressed ranks.
  auto kth = mo::RangeKth(a, {{0, 4, 2}, {1, 5, 4}});
  std::cout << kth[0] << ' ' << kth[1] << '\n';  // 2 4
}
