#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <vector>

struct KnapsackOption {
  int weight;
  long long value;
};

inline std::optional<long long> GroupedKnapsack(
    const std::vector<std::vector<KnapsackOption>>& groups, int capacity) {
  const auto size = static_cast<std::size_t>(capacity) + 1;
  std::vector<std::optional<long long>> dp(size, 0), next(size);

  for (const auto& group : groups) {
    std::fill(next.begin(), next.end(), std::nullopt);
    for (const auto& [weight, value] : group) {
      for (auto c = static_cast<std::size_t>(weight); c < size; ++c) {
        const auto& previous = dp[c - weight];
        if (!previous) continue;
        const long long candidate = *previous + value;
        if (!next[c] || candidate > *next[c]) next[c] = candidate;
      }
    }
    dp.swap(next);
  }
  return dp.back();
}
