#pragma once

#include <cstdint>

namespace nt {

// k >= 1. Returns the largest r with r^k <= n, without floating point.
inline std::uint64_t FloorKthRoot(std::uint64_t n, unsigned k) {
  if (n < 2 || k == 1) return n;
  if (k >= 64) return 1;

  const auto fits = [n, k](std::uint64_t x) {
    std::uint64_t product = 1;
    for (unsigned i = 0; i < k; ++i) {
      if (product > n / x) return false;
      product *= x;
    }
    return true;
  };

  std::uint64_t lo = 1, hi = n;
  while (lo < hi) {
    const auto gap = hi - lo;
    const auto mid = lo + gap / 2 + gap % 2;
    if (fits(mid)) lo = mid;
    else hi = mid - 1;
  }
  return lo;
}

}  // namespace nt
