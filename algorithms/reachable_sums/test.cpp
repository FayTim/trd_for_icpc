#include "reachable_sums.h"

#include <bitset>
#include <cstddef>
#include <stdexcept>
#include <vector>

void CheckAgainstSubsets(const std::vector<int>& weights) {
  constexpr int limit = 65;
  std::bitset<limit + 1> expected;
  const auto subset_count = std::size_t{1} << weights.size();
  for (std::size_t mask = 0; mask < subset_count; ++mask) {
    int sum = 0;
    for (std::size_t i = 0; i < weights.size(); ++i) {
      if ((mask >> i) & 1U) {
        sum += weights[i];
      }
    }
    if (sum <= limit) {
      expected.set(sum);
    }
  }
  if (ReachableSums<limit>(weights) != expected) {
    throw std::runtime_error("Reachable sums differ from exhaustive search");
  }
}

int main() {
  CheckAgainstSubsets({});
  CheckAgainstSubsets({2});
  CheckAgainstSubsets({2, 3, 3});
  CheckAgainstSubsets({0, 0, 1, 3});
  CheckAgainstSubsets({1, 2, 63, 65, 100});
  if (ReachableSums<0>({0, 2}).count() != 1) {
    throw std::runtime_error("Sum zero must be reachable");
  }
}
