#include "fenwick_tree.h"

#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

int main() {
  const FenwickTree empty(0);
  if (empty.Sum(0, 0) != 0) {
    std::cerr << "Empty tree should have sum zero\n";
    return 1;
  }

  FenwickTree tree(7);
  std::vector<long long> values(7);
  const std::vector<std::pair<int, long long>> updates{
      {0, 4'000'000'000LL}, {6, 8}, {3, 5}, {1, 2}, {0, -7}, {6, -8}};

  for (const auto& [i, delta] : updates) {
    tree.Add(i, delta);
    values[i] += delta;
    for (int l = 0; l <= 7; ++l) {
      for (int r = l; r <= 7; ++r) {
        const long long expected =
            std::accumulate(values.begin() + l, values.begin() + r, 0LL);
        if (tree.Sum(l, r) != expected) {
          std::cerr << "Incorrect sum for [" << l << ", " << r << ")\n";
          return 1;
        }
      }
    }
  }
}
