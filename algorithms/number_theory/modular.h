#pragma once

#include <cstdint>
#include <optional>
#include <utility>

namespace nt {

struct Bezout {
  std::int64_t gcd, x, y;
};

// a, b >= 0; at least one is positive. Returns a*x + b*y = gcd.
inline Bezout ExtendedGcd(std::int64_t a, std::int64_t b) {
  if (b == 0) return {a, 1, 0};
  const auto [g, x, y] = ExtendedGcd(b, a % b);
  return {g, y, static_cast<std::int64_t>(
                    x - static_cast<__int128_t>(a / b) * y)};
}

// m > 0. Use a wide argument for products and differences before reduction.
inline std::int64_t NormalizeMod(__int128_t a, std::int64_t m) {
  a %= m;
  return static_cast<std::int64_t>(a < 0 ? a + m : a);
}

inline std::optional<std::int64_t> ModInverse(std::int64_t a,
                                              std::int64_t m) {
  const auto [g, x, y] = ExtendedGcd(NormalizeMod(a, m), m);
  if (g != 1) return std::nullopt;
  return NormalizeMod(x, m);
}

// Solve a*x = b (mod m), m > 0. Result (r, period): x = r (mod period).
inline std::optional<std::pair<std::int64_t, std::int64_t>>
SolveCongruence(std::int64_t a, std::int64_t b, std::int64_t m) {
  const auto [g, x, y] = ExtendedGcd(NormalizeMod(a, m), m);
  if (b % g != 0) return std::nullopt;
  const auto period = m / g;
  return std::pair{NormalizeMod(static_cast<__int128_t>(x) * (b / g),
                                period), period};
}

// x = a (mod m), x = b (mod n); m,n > 0; lcm(m,n) <= INT64_MAX.
// Empty result means inconsistent congruences, otherwise x = r (mod lcm).
inline std::optional<std::pair<std::int64_t, std::int64_t>>
MergeCongruences(std::int64_t a, std::int64_t m,
                 std::int64_t b, std::int64_t n) {
  a = NormalizeMod(a, m);
  b = NormalizeMod(b, n);
  const auto [g, x, y] = ExtendedGcd(m, n);
  const __int128_t difference = static_cast<__int128_t>(b) - a;
  if (difference % g != 0) return std::nullopt;
  const auto k = NormalizeMod(difference / g * x, n / g);
  const auto period = static_cast<std::int64_t>(
      static_cast<__int128_t>(m / g) * n);
  return std::pair{NormalizeMod(static_cast<__int128_t>(m) * k + a,
                                period), period};
}

}  // namespace nt
