#pragma once

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

namespace nt {
using u64 = std::uint64_t;
using Factors = std::vector<std::pair<u64, int>>;

// GCC/Clang; mod > 0. The product is exact for all uint64_t inputs.
inline u64 MulMod(u64 a, u64 b, u64 mod) {
  return static_cast<__uint128_t>(a) * b % mod;
}
inline u64 PowMod(u64 a, u64 exponent, u64 mod) {
  u64 result = 1 % mod;
  for (; exponent; exponent >>= 1, a = MulMod(a, a, mod))
    if (exponent & 1) result = MulMod(result, a, mod);
  return result;
}

inline bool IsPrime(u64 n) {
  if (n < 2) return false;
  for (u64 p : {2, 3, 5, 7})
    if (n % p == 0) return n == p;
  u64 d = n - 1;
  int s = 0;
  while (!(d & 1)) { d >>= 1; ++s; }
  for (u64 a : {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL,
                9780504ULL, 1795265022ULL}) {
    if (a % n == 0) continue;
    u64 x = PowMod(a, d, n);
    if (x == 1 || x == n - 1) continue;
    int r = 1;
    for (; r < s; ++r) {
      x = MulMod(x, x, n);
      if (x == n - 1) break;
    }
    if (r == s) return false;
  }
  return true;
}

namespace detail {
// n is composite. Brent's cycle search, with batched gcd and restarts.
inline u64 Pollard(u64 n) {
  if (n % 2 == 0) return 2;
  if (n % 3 == 0) return 3;
  static thread_local std::mt19937_64 random(std::random_device{}());
  const auto difference = [](u64 a, u64 b) { return a > b ? a-b : b-a; };
  for (;;) {
    const u64 c = random() % (n - 1) + 1;
    auto step = [&](u64 x) {
      return static_cast<u64>((static_cast<__uint128_t>(x) * x + c) % n);
    };
    u64 y = random() % (n - 1) + 1, x = 0, saved = 0, g = 1;
    for (u64 span = 1; g == 1 && span <= (1 << 20); span *= 2) {
      x = y;
      for (u64 i = 0; i < span; ++i) y = step(y);
      for (u64 k = 0; k < span && g == 1; k += 64) {
        saved = y;
        u64 product = 1;
        for (u64 i = 0; i < std::min<u64>(64, span - k); ++i) {
          y = step(y);
          product = MulMod(product, difference(x, y), n);
        }
        g = std::gcd(product, n);
      }
    }
    if (g == n) {
      do {
        saved = step(saved);
        g = std::gcd(difference(x, saved), n);
      } while (g == 1);
    }
    if (g > 1 && g < n) return g;
  }
}
inline void CollectFactors(u64 n, std::vector<u64>& result) {
  if (n == 1) return;
  if (IsPrime(n)) { result.push_back(n); return; }
  const u64 d = Pollard(n);
  CollectFactors(d, result);
  CollectFactors(n / d, result);
}
}  // namespace detail

// n >= 1; ascending distinct primes with their positive exponents.
inline Factors Factorize(u64 n) {
  std::vector<u64> primes;
  detail::CollectFactors(n, primes);
  std::sort(primes.begin(), primes.end());
  Factors result;
  for (u64 p : primes) {
    if (result.empty() || result.back().first != p) result.emplace_back(p, 0);
    ++result.back().second;
  }
  return result;
}

// Input below is a factorization of 1 <= n <= UINT64_MAX; {} means 1.
inline std::vector<u64> Divisors(const Factors& factors) {
  std::vector<u64> result{1};
  for (auto [p, exponent] : factors) {
    const auto size = result.size();
    u64 power = 1;
    for (int e = 1; e <= exponent; ++e) {
      power *= p;
      for (std::size_t i = 0; i < size; ++i) result.push_back(result[i] * power);
    }
  }
  return result;  // Not sorted.
}
inline u64 Totient(const Factors& factors) {
  u64 result = 1;
  for (auto [p, exponent] : factors) {
    result *= p - 1;
    for (int e = 1; e < exponent; ++e) result *= p;
  }
  return result;
}
inline u64 DivisorCount(const Factors& factors) {
  u64 result = 1;
  for (auto [p, exponent] : factors) result *= exponent + 1;
  return result;
}
// Precondition: the mathematical sum of divisors fits uint64_t.
inline u64 DivisorSum(const Factors& factors) {
  u64 result = 1;
  for (auto [p, exponent] : factors) {
    u64 power = 1, sum = 1;
    for (int e = 1; e <= exponent; ++e) { power *= p; sum += power; }
    result *= sum;
  }
  return result;
}
}  // namespace nt
