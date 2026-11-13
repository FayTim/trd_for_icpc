#include "knapsack_01.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

void CheckAgainstSubsets(const std::vector<int>& weights,
                         const std::vector<long long>& values) {
  constexpr int limit = 65;
  std::vector<long long> best(limit + 1, 0);
  const auto subset_count = std::size_t{1} << weights.size();
  for (std::size_t mask = 0; mask < subset_count; ++mask) {
    int weight = 0;
    long long value = 0;
    for (std::size_t i = 0; i < weights.size(); ++i) {
      if ((mask >> i) & 1U) {
        weight += weights[i];
        value += values[i];
      }
    }
    for (int capacity = weight; capacity <= limit; ++capacity) {
      best[capacity] = std::max(best[capacity], value);
    }
  }
  for (int capacity = 0; capacity <= limit; ++capacity) {
    if (Knapsack01(weights, values, capacity) != best[capacity]) {
      throw std::runtime_error("Knapsack optimum differs from exhaustive search");
    }
  }
}

int main() {
  CheckAgainstSubsets({}, {});
  CheckAgainstSubsets({2}, {5});
  CheckAgainstSubsets({2, 3, 3}, {5, 6, 7});
  CheckAgainstSubsets({0, 0, 1, 3}, {8, -4, -5, 6});
  CheckAgainstSubsets({1, 2, 63, 65, 100}, {8, 3, 4, 5, 200});
}
