#include "divisibility.h"

#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>

namespace {
void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

void TestTransforms() {
  Require(nt::InvertMultiples({}).empty(), "Empty inverse multiples");
  Require(nt::InvertDivisors({}).empty(), "Empty inverse divisors");
  std::mt19937 random(1743);
  for (int n = 0; n <= 100; ++n) {
    std::vector<std::int64_t> values(n + 1), multiples(n + 1), divisors(n + 1);
    for (int i = 1; i <= n; ++i) values[i] = static_cast<int>(random() % 21) - 10;
    for (int i = 1; i <= n; ++i)
      for (int j = 1; j <= n; ++j)
        if (j % i == 0) { multiples[i] += values[j]; divisors[j] += values[i]; }
    Require(nt::SumOverMultiples(values) == multiples, "Brute multiples");
    Require(nt::SumOverDivisors(values) == divisors, "Brute divisors");
    Require(nt::InvertMultiples(multiples) == values, "Inverse multiples");
    Require(nt::InvertDivisors(divisors) == values, "Inverse divisors");
  }
}

void TestGcdPairs() {
  std::mt19937 random(5329);
  for (int trial = 0; trial < 1000; ++trial) {
    std::vector<int> values(random() % 30);
    std::vector<std::int64_t> frequency(31), expected(31);
    for (int& x : values) { x = 1 + random() % 30; ++frequency[x]; }
    for (std::size_t i = 0; i < values.size(); ++i)
      for (std::size_t j = 0; j < i; ++j) ++expected[std::gcd(values[i], values[j])];
    Require(nt::ExactGcdPairs(frequency) == expected, "Brute gcd pairs");
  }
  const std::int64_t total = std::int64_t{1} << 32;
  Require(nt::ExactGcdPairs({0, total})[1] == (total / 2) * (total - 1),
          "Pair count at supported limit");
  const auto half = total / 2;
  const auto mixed = nt::ExactGcdPairs({0, half, half});
  Require(mixed[2] == (half / 2) * (half - 1), "Large equal-value pairs");
  Require(mixed[1] == half * half + mixed[2], "Large mixed gcd inversion");
}

void TestValuationsAndBlocks() {
  for (std::uint64_t p : {2, 3, 5, 7, 11}) {
    std::uint64_t expected = 0;
    Require(nt::FactorialValuation(0, p) == 0, "Zero factorial");
    for (std::uint64_t n = 1; n <= 1000; ++n) {
      std::uint64_t exponent = 0, power = p;
      while (power <= n && n % power == 0) { ++exponent; power *= p; }
      Require(nt::Valuation(n, p) == exponent, "Brute valuation");
      expected += exponent;
      Require(nt::FactorialValuation(n, p) == expected, "Brute factorial valuation");
    }
  }
  const auto maximum = std::numeric_limits<std::uint64_t>::max();
  Require(nt::FactorialValuation(maximum, 2) == maximum - 64, "Large factorial");
  Require(nt::Valuation(std::uint64_t{1} << 63, 2) == 63, "Large power");
  for (std::uint64_t n = 0; n <= 1000; ++n) {
    std::uint64_t next = 1, sum = 0, expected = 0;
    nt::QuotientBlocks(n, [&](auto l, auto r, auto q) {
      Require(l == next && r >= l && r <= n, "Block coverage");
      for (auto i = l; i <= r; ++i) Require(n / i == q, "Block quotient");
      sum += (r - l + 1) * q;
      next = r + 1;
    });
    for (std::uint64_t d = 1; d <= n; ++d) expected += n / d;
    Require(sum == expected && next == n + 1, "Brute floor sum");
  }
}
}  // namespace

int main() {
  try { TestTransforms(); TestGcdPairs(); TestValuationsAndBlocks(); }
  catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
  std::cout << "Divisibility tests passed\n";
}
