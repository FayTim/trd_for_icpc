#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

inline long long Knapsack01(const std::vector<int>& weights,
                            const std::vector<long long>& values, int capacity) {
  std::vector<long long> dp(static_cast<std::size_t>(capacity) + 1, 0);
  for (std::size_t i = 0; i < weights.size(); ++i) {
    const int w = weights[i];
    for (int c = capacity; c >= w; --c) {
      dp[c] = std::max(dp[c], dp[c - w] + values[i]);
    }
  }
  return dp[capacity];
}
