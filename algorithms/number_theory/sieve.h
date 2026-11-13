#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace nt {

struct Sieve {
  std::vector<int> spf, primes, phi, mu;

  // 0 <= limit < INT_MAX; arrays cover [0, limit].
  explicit Sieve(int limit)
      : spf(static_cast<std::size_t>(limit) + 1),
        phi(spf.size()), mu(spf.size()) {
    if (limit >= 1) phi[1] = mu[1] = 1;
    for (int i = 2; i <= limit; ++i) {
      if (spf[i] == 0) {
        spf[i] = i;
        primes.push_back(i);
        phi[i] = i - 1;
        mu[i] = -1;
      }
      for (int p : primes) {
        if (p > limit / i) break;
        spf[i * p] = p;
        if (i % p == 0) {
          phi[i * p] = phi[i] * p;
          mu[i * p] = 0;
          break;
        }
        phi[i * p] = phi[i] * (p - 1);
        mu[i * p] = -mu[i];
      }
    }
  }

  // 1 <= n <= limit; prime factors are returned in increasing order.
  std::vector<std::pair<std::uint64_t, int>> Factorize(int n) const {
    std::vector<std::pair<std::uint64_t, int>> result;
    while (n > 1) {
      const int p = spf[n];
      int exponent = 0;
      do {
        n /= p;
        ++exponent;
      } while (n % p == 0);
      result.emplace_back(p, exponent);
    }
    return result;
  }
};

// 0 <= left <= right; both sqrt(right) and interval storage must fit memory.
inline std::vector<std::int64_t> SegmentedPrimes(std::int64_t left,
                                               std::int64_t right) {
  auto root = static_cast<std::int64_t>(std::sqrt((long double)right));
  while (root + 1 <= right / (root + 1)) ++root;
  while (root > 0 && root > right / root) --root;
  std::vector<bool> base(static_cast<std::size_t>(root) + 1, true);
  std::vector<bool> prime(static_cast<std::size_t>(right - left) + 1, true);
  for (std::int64_t p = 2; p <= root; ++p) {
    if (!base[p]) continue;
    for (std::int64_t x = p * p; x <= root; x += p) base[x] = false;
    const auto offset = (p - left % p) % p;
    if (offset > right - left) continue;
    const auto start = std::max(p * p, left + offset);
    for (auto x = start; x <= right;) {
      prime[x - left] = false;
      if (right - x < p) break;  // Do not overflow at INT64_MAX.
      x += p;
    }
  }
  std::vector<std::int64_t> result;
  for (std::size_t i = 0; i < prime.size(); ++i) {
    const auto x = left + static_cast<std::int64_t>(i);
    if (x >= 2 && prime[i]) result.push_back(x);
  }
  return result;
}

}  // namespace nt
