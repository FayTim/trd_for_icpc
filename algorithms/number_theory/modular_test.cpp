#include "modular.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace {

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

void CheckBezout(std::int64_t a, std::int64_t b) {
  const auto [g, x, y] = nt::ExtendedGcd(a, b);
  Require(g == std::gcd(a, b), "Wrong gcd");
  Require(static_cast<__int128_t>(a) * x +
              static_cast<__int128_t>(b) * y == g,
          "Wrong Bezout coefficients");
}

void TestExhaustive() {
  for (std::int64_t a = 0; a <= 100; ++a) {
    for (std::int64_t b = 0; b <= 100; ++b) {
      if (a != 0 || b != 0) CheckBezout(a, b);
    }
  }
  for (std::int64_t m = 1; m <= 30; ++m) {
    for (std::int64_t a = -40; a <= 40; ++a) {
      const auto inverse = nt::ModInverse(a, m);
      std::int64_t expected_inverse = -1;
      for (std::int64_t x = 0; x < m; ++x) {
        if (nt::NormalizeMod(a * x - 1, m) == 0) expected_inverse = x;
      }
      Require(inverse.has_value() == (expected_inverse != -1),
              "Inverse existence disagrees with brute force");
      if (inverse) Require(*inverse == expected_inverse, "Wrong inverse");
      for (std::int64_t b = -40; b <= 40; ++b) {
        const auto solution = nt::SolveCongruence(a, b, m);
        int count = 0;
        for (std::int64_t x = 0; x < m; ++x) {
          const bool expected = nt::NormalizeMod(a * x - b, m) == 0;
          const bool actual = solution &&
              nt::NormalizeMod(x - solution->first, solution->second) == 0;
          Require(actual == expected, "Congruence disagrees with brute force");
          count += expected;
        }
        Require(solution.has_value() == (count != 0), "Wrong existence");
        if (solution) {
          Require(solution->second == m / count, "Wrong solution period");
          Require(solution->first >= 0 && solution->first < solution->second,
                  "Non-canonical solution");
        }
      }
    }
  }
  for (std::int64_t m = 1; m <= 15; ++m) {
    for (std::int64_t n = 1; n <= 15; ++n) {
      const auto period = std::lcm(m, n);
      for (std::int64_t a = -m; a <= m; ++a) {
        for (std::int64_t b = -n; b <= n; ++b) {
          const auto merged = nt::MergeCongruences(a, m, b, n);
          std::int64_t expected = -1;
          for (std::int64_t x = 0; x < period; ++x) {
            if ((x - a) % m == 0 && (x - b) % n == 0) expected = x;
          }
          Require(merged.has_value() == (expected != -1),
                  "CRT existence disagrees with brute force");
          if (merged) Require(merged->first == expected &&
                                  merged->second == period, "Wrong CRT result");
        }
      }
    }
  }
}

void TestLarge() {
  constexpr auto high = std::numeric_limits<std::int64_t>::max();
  constexpr auto low = std::numeric_limits<std::int64_t>::min();
  CheckBezout(high, high - 1);
  CheckBezout(high, 0);
  CheckBezout(0, high);
  CheckBezout(7540113804746346429LL, 4660046610375530309LL);
  Require(nt::NormalizeMod(low, high) == high - 1, "INT64_MIN reduction");
  Require(*nt::ModInverse(low, high) == high - 1, "INT64_MIN inverse");
  const auto linear = nt::SolveCongruence(low, low, high);
  Require(linear && linear->first == 1 && linear->second == high,
          "Large linear congruence");
  const auto trivial = nt::SolveCongruence(0, 0, high);
  Require(trivial && *trivial == std::pair<std::int64_t, std::int64_t>{0, 1},
          "All integers should solve the zero equation");
  Require(!nt::SolveCongruence(0, 1, high), "Inconsistent zero equation");
  const auto equal = nt::MergeCongruences(low, high, high - 1, high);
  Require(equal && equal->first == high - 1 && equal->second == high,
          "Large equal CRT moduli");
  constexpr std::int64_t m = 2147483647, n = 4294967291;
  const auto merged = nt::MergeCongruences(low, m, high, n);
  Require(merged && merged->second == m * n, "Large CRT period");
  Require(nt::NormalizeMod(static_cast<__int128_t>(merged->first) - low, m) == 0 &&
              nt::NormalizeMod(static_cast<__int128_t>(merged->first) - high, n) == 0,
          "Large CRT residues");
}

}  // namespace

int main() {
  try {
    TestExhaustive();
    TestLarge();
  } catch (const std::exception& error) {
    std::cerr << "Modular test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "Modular tests passed\n";
}
