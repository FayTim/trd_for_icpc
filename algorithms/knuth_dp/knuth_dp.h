#pragma once

#include <algorithm>
#include <vector>

template <class Cost>
long long KnuthDP(int n, Cost cost) {
  if (n < 2) return 0;

  std::vector dp(n, std::vector<long long>(n, 0));
  std::vector opt(n, std::vector<int>(n));
  for (int i = 0; i < n; ++i) opt[i][i] = i;

  for (int len = 2; len <= n; ++len) {
    for (int l = 0; l <= n - len; ++l) {
      const int r = l + len - 1;
      const int lo = opt[l][r - 1];
      const int hi = std::min(r - 1, opt[l + 1][r]);
      int best = lo;
      for (int k = lo + 1; k <= hi; ++k) {
        if (dp[l][k] + dp[k + 1][r] < dp[l][best] + dp[best + 1][r]) {
          best = k;
        }
      }
      opt[l][r] = best;
      dp[l][r] = dp[l][best] + dp[best + 1][r] + cost(l, r);
    }
  }
  return dp[0][n - 1];
}
