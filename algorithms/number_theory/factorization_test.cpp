#include "factorization.h"

#include <iostream>
#include <limits>
#include <stdexcept>

namespace {
using nt::u64;
using U128 = __uint128_t;

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

nt::Factors TrialFactor(u64 n) {
  nt::Factors result;
  for (u64 p = 2; p <= n / p; ++p) {
    if (n % p != 0) continue;
    int exponent = 0;
    do { n /= p; ++exponent; } while (n % p == 0);
    result.emplace_back(p, exponent);
  }
  if (n > 1) result.emplace_back(n, 1);
  return result;
}

void CheckFactorization(u64 n) {
  const auto factors = nt::Factorize(n);
  U128 product = 1;
  u64 previous = 0;
  for (auto [p, exponent] : factors) {
    Require(p > previous && nt::IsPrime(p), "Factors must be ordered primes");
    Require(exponent > 0, "Exponents must be positive");
    for (int i = 0; i < exponent; ++i) product *= p;
    previous = p;
  }
  Require(product == n, "Factor product differs from the input");
  const auto divisors = nt::Divisors(factors);
  Require(divisors.size() == nt::DivisorCount(factors), "Wrong divisor count");
  U128 totient_sum = 0;
  for (u64 d : divisors) {
    Require(n % d == 0, "Returned number is not a divisor");
    nt::Factors part;
    for (auto [p, exponent] : factors) {
      int used = 0;
      while (d % p == 0) { d /= p; ++used; }
      if (used) part.emplace_back(p, used);
    }
    totient_sum += nt::Totient(part);
  }
  Require(totient_sum == n, "Sum of totients over divisors must equal n");
}

void TestSmallNumbers() {
  constexpr int limit = 200000;
  std::vector<bool> prime(limit + 1, true);
  prime[0] = prime[1] = false;
  for (int i = 2; i * i <= limit; ++i)
    if (prime[i])
      for (int j = i * i; j <= limit; j += i) prime[j] = false;
  for (int n = 0; n <= limit; ++n)
    Require(nt::IsPrime(n) == prime[n], "Primality differs from the sieve");
  for (u64 n = 1; n <= 10000; ++n)
    Require(nt::Factorize(n) == TrialFactor(n), "Factorization differs from trial division");
  for (u64 n = 1; n <= 500; ++n) {
    std::vector<u64> expected;
    u64 phi = 0, sum = 0;
    for (u64 d = 1; d <= n; ++d) {
      phi += std::gcd(n, d) == 1;
      if (n % d == 0) { expected.push_back(d); sum += d; }
    }
    auto factors = nt::Factorize(n);
    auto actual = nt::Divisors(factors);
    std::sort(actual.begin(), actual.end());
    Require(actual == expected, "Divisor list differs from enumeration");
    Require(nt::Totient(factors) == phi, "Totient differs from gcd enumeration");
    Require(nt::DivisorCount(factors) == expected.size(), "Wrong divisor count");
    Require(nt::DivisorSum(factors) == sum, "Wrong divisor sum");
  }
}

void TestFullWidth() {
  for (u64 n : {561ULL, 1105ULL, 1729ULL, 3215031751ULL,
                341550071728321ULL, 3825123056546413051ULL})
    Require(!nt::IsPrime(n), "Pseudoprime accepted");
  for (u64 n : {4294967279ULL, 4294967291ULL, 2305843009213693951ULL,
                18446744073709551557ULL})
    Require(nt::IsPrime(n), "Known prime rejected");
  for (u64 n : {1ULL, 4ULL, 25ULL, 121ULL, 1000000016000000063ULL,
                9223372036854775808ULL, 12157665459056928801ULL,
                7450580596923828125ULL, 18446744030759878681ULL,
                18446743979220271189ULL, 18446744073709551557ULL,
                18446744073709551615ULL})
    CheckFactorization(n);
  const u64 p = 4294967291ULL, q = 4294967279ULL;
  Require(nt::Factorize(p * p) == nt::Factors{{p, 2}}, "Wrong large square factors");
  Require(nt::Factorize(p * q) == nt::Factors{{q, 1}, {p, 1}}, "Wrong semiprime factors");
  Require(nt::DivisorSum({{2, 63}}) == std::numeric_limits<u64>::max(),
          "Divisor sum at uint64 boundary failed");
  std::mt19937_64 random(0xFA6702);
  for (int i = 0; i < 100; ++i) CheckFactorization(random() | 1);
}

void TestModularArithmetic() {
  const auto add = [](u64 a, u64 b, u64 mod) {
    return a >= mod - b ? a - (mod - b) : a + b;
  };
  const auto multiply = [&](u64 a, u64 b, u64 mod) {
    u64 result = 0;
    a %= mod;
    for (; b; b >>= 1, a = add(a, a, mod))
      if (b & 1) result = add(result, a, mod);
    return result;
  };
  std::mt19937_64 random(0xA11CE);
  for (int i = 0; i < 2000; ++i) {
    const u64 a = random(), b = random(), mod = random() | 1;
    Require(nt::MulMod(a, b, mod) == multiply(a, b, mod),
            "Full-width modular multiplication failed");
    const u64 exponent = random() % 100;
    u64 expected = 1 % mod;
    for (u64 j = 0; j < exponent; ++j)
      expected = multiply(expected, a, mod);
    Require(nt::PowMod(a, exponent, mod) == expected, "Modular power failed");
  }
  Require(nt::PowMod(0, 0, 1) == 0 && nt::PowMod(1, 0, 2) == 1,
          "Zero exponent or modulus one failed");
}
}  // namespace

int main() {
  try {
    TestSmallNumbers();
    TestFullWidth();
    TestModularArithmetic();
  } catch (const std::exception& error) {
    std::cerr << "Factorization test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "Factorization tests passed\n";
}
