#pragma once

#include <algorithm>
#include <bit>
#include <cstddef>
#include <limits>
#include <vector>

inline long long AssignmentCost(const std::vector<std::vector<int>>& cost) {
  const std::size_t n = cost.size();
  const std::size_t states = std::size_t{1} << n;
  std::vector<long long> dp(states, std::numeric_limits<long long>::max());
  dp[0] = 0;

  for (std::size_t mask = 0; mask < states - 1; ++mask) {
    const int person = std::popcount(mask);
    for (std::size_t seat = 0; seat < n; ++seat) {
      const std::size_t bit = std::size_t{1} << seat;
      if (mask & bit) continue;

      const std::size_t next = mask | bit;
      dp[next] = std::min(dp[next], dp[mask] + cost[person][seat]);
    }
  }
  return dp.back();
}
