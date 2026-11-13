#include "integer_roots.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>

namespace {
using U64 = std::uint64_t;
using U128 = __uint128_t;
constexpr U64 kMax = std::numeric_limits<U64>::max();

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

// Independent oracle: multiply in the wider type and cap above uint64_t.
U128 CappedPower(U128 base, unsigned k) {
  U128 value = 1;
  for (unsigned i = 0; i < k; ++i) {
    value *= base;
    if (value > kMax) return U128{kMax} + 1;
  }
  return value;
}

void Check(U64 n, unsigned k) {
  const auto r = nt::FloorKthRoot(n, k);
  Require(CappedPower(r, k) <= n, "Returned root is too large");
  Require(CappedPower(U128{r} + 1, k) > n, "Returned root is too small");
}

void TestSmallAndPowerBoundaries() {
  for (U64 n = 0; n <= 500; ++n) {
    for (unsigned k = 1; k <= 12; ++k) {
      U64 expected = 0;
      while (CappedPower(U128{expected} + 1, k) <= n) ++expected;
      Require(nt::FloorKthRoot(n, k) == expected,
              "Root differs from exhaustive search");
    }
  }
  for (U64 base = 2; base <= 100; ++base) {
    for (unsigned k = 2; k <= 63; ++k) {
      const U128 power = CappedPower(base, k);
      if (power > kMax) break;
      const auto n = static_cast<U64>(power);
      Require(nt::FloorKthRoot(n - 1, k) == base - 1,
              "Boundary immediately below a power failed");
      Require(nt::FloorKthRoot(n, k) == base, "Exact power failed");
      if (n < kMax) Check(n + 1, k);
    }
  }
}

void TestFullWidth() {
  Require(nt::FloorKthRoot(kMax, 1) == kMax, "First root failed");
  Require(nt::FloorKthRoot(kMax, 2) == 4294967295ULL,
          "Full-width square root failed");
  Require(nt::FloorKthRoot(kMax, 63) == 2, "63rd root failed");
  for (unsigned k : {64U, 65U, std::numeric_limits<unsigned>::max()}) {
    Require(nt::FloorKthRoot(0, k) == 0, "Zero boundary failed");
    Require(nt::FloorKthRoot(1, k) == 1, "One boundary failed");
    Require(nt::FloorKthRoot(kMax, k) == 1, "Large exponent failed");
  }
  std::mt19937_64 random(0xB007);
  for (int i = 0; i < 10000; ++i) {
    const U64 n = random();
    const auto k = static_cast<unsigned>(1 + random() % 80);
    Check(n, k);
  }
}
}  // namespace

int main() {
  try {
    TestSmallAndPowerBoundaries();
    TestFullWidth();
  } catch (const std::exception& error) {
    std::cerr << "Integer root test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "Integer root tests passed\n";
}
