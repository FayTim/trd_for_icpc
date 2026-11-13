#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

inline long long MergePilesCost(const std::vector<int>& piles) {
  const std::size_t n = piles.size();
  if (n < 2) return 0;

  std::vector<long long> prefix(n + 1, 0);
  for (std::size_t i = 0; i < n; ++i) {
    prefix[i + 1] = prefix[i] + piles[i];
  }

  std::vector dp(n, std::vector<long long>(n, 0));
  for (std::size_t len = 2; len <= n; ++len) {
    for (std::size_t l = 0; l <= n - len; ++l) {
      const std::size_t r = l + len - 1;
      dp[l][r] = std::numeric_limits<long long>::max();
      for (std::size_t k = l; k < r; ++k) {
        dp[l][r] = std::min(dp[l][r], dp[l][k] + dp[k + 1][r]);
      }
      dp[l][r] += prefix[r + 1] - prefix[l];
    }
  }
  return dp[0][n - 1];
}
