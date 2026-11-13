#include "binomial.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {
using U64 = std::uint64_t;
using U128 = __uint128_t;
constexpr U64 kMax = std::numeric_limits<U64>::max();

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

void TestExactAgainstPascal() {
  // Saturating Pascal is an independent addition-only oracle.
  std::vector<U128> row{1};
  for (int n = 0; n <= 200; ++n) {
    for (int k = 0; k <= n; ++k) {
      const auto actual = combinatorics::Binomial(n, k);
      Require(actual.has_value() == (row[k] <= kMax),
              "Incorrect overflow detection");
      if (actual) Require(*actual == row[k], "Exact coefficient differs from Pascal");
    }
    Require(combinatorics::Binomial(n, n + 1) == 0, "k > n must give zero");
    row.push_back(0);
    for (int k = n + 1; k >= 1; --k)
      row[k] = std::min(U128{kMax} + 1, row[k] + row[k - 1]);
  }
  Require(combinatorics::Binomial(67, 33) == 14226520737620288370ULL,
          "Largest fully fitting row failed");
  Require(!combinatorics::Binomial(68, 34), "Overflow boundary failed");
  Require(combinatorics::Binomial(kMax, 0) == 1, "C(UINT64_MAX,0) failed");
  Require(combinatorics::Binomial(kMax, 1) == kMax, "Full-width n failed");
  Require(combinatorics::Binomial(kMax, kMax - 1) == kMax,
          "Full-width symmetry failed");
  Require(combinatorics::Binomial(kMax, kMax) == 1,
          "C(UINT64_MAX,UINT64_MAX) failed");
  Require(!combinatorics::Binomial(kMax, 2), "Large coefficient must overflow");
  Require(!combinatorics::Binomial(kMax, kMax / 2),
          "Huge k must stop at the first overflow");
}

void CheckModulo(int max_n, U64 mod) {
  const combinatorics::PrimeBinomial table(max_n, mod);
  std::vector<U64> row(static_cast<std::size_t>(max_n) + 1);
  row[0] = 1;
  for (int n = 0; n <= max_n; ++n) {
    U64 sum = 0;
    for (int k = 0; k <= n; ++k) {
      Require(table.Choose(n, k) == row[k], "Modular coefficient differs from Pascal");
      sum = static_cast<U64>((U128{sum} + row[k]) % mod);
    }
    Require(sum == nt::PowMod(2, n, mod), "Row sum is not 2^n modulo p");
    Require(table.Choose(n, -1) == 0 && table.Choose(n, n + 1) == 0,
            "Out-of-range k must give zero");
    if (n == max_n) break;
    for (int k = n + 1; k >= 1; --k)
      row[k] = static_cast<U64>((U128{row[k]} + row[k - 1]) % mod);
  }
}

void TestPrimeTables() {
  CheckModulo(0, 2);
  for (int prime : {2, 3, 5, 7, 17, 101}) CheckModulo(prime - 1, prime);
  CheckModulo(1000, 1'000'000'007);
  CheckModulo(100, 18446744073709551557ULL);
}
}  // namespace

int main() {
  try {
    TestExactAgainstPascal();
    TestPrimeTables();
  } catch (const std::exception& error) {
    std::cerr << "Combinatorics test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "Combinatorics tests passed\n";
}
