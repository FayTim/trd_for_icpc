#include "fenwick_tree.h"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

int main() {
  // (x, is_query, y_rank). Updates come first when x is equal.
  std::vector<std::tuple<int, bool, int>> events{
      {1, false, 1}, {2, false, 0}, {2, false, 1}, {2, true, 1}};
  std::sort(events.begin(), events.end());

  FenwickTree frequencies(2);
  long long pairs = 0;
  for (const auto& [x, is_query, y_rank] : events) {
    if (is_query) {
      pairs += frequencies.PrefixSum(y_rank + 1);
    } else {
      frequencies.Add(y_rank, 1);
    }
  }
  std::cout << pairs << '\n';
}
