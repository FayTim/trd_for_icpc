#pragma once

#include <algorithm>
#include <numeric>
#include <vector>

template <class Cost>
long long DivideConquerDP(int n, int groups, Cost cost) {
  std::vector<long long> prev(n + 1), next(n + 1);
  for (int r = 1; r <= n; ++r) prev[r] = cost(0, r);

  const auto solve = [&](auto&& self, int l, int r, int lo, int hi) -> void {
    if (l > r) return;
    const int mid = std::midpoint(l, r);
    int best = lo;
    long long value = prev[lo] + cost(lo, mid);
    for (int k = lo + 1; k <= std::min(hi, mid - 1); ++k) {
      const long long candidate = prev[k] + cost(k, mid);
      if (candidate < value) {
        value = candidate;
        best = k;
      }
    }
    next[mid] = value;
    self(self, l, mid - 1, lo, best);
    self(self, mid + 1, r, best, hi);
  };

  for (int g = 2; g <= groups; ++g) {
    solve(solve, g, n, g - 1, n - 1);
    prev.swap(next);
  }
  return prev[n];
}
