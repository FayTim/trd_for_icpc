#include "sieve.h"

#include <cstdint>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

bool TrialPrime(std::int64_t n) {
  if (n < 2) return false;
  if (n % 2 == 0) return n == 2;
  for (std::int64_t d = 3; d <= n / d; d += 2) {
    if (n % d == 0) return false;
  }
  return true;
}

std::vector<std::pair<std::uint64_t, int>> TrialFactor(int n) {
  std::vector<std::pair<std::uint64_t, int>> result;
  for (int d = 2; d <= n / d; ++d) {
    int exponent = 0;
    while (n % d == 0) {
      n /= d;
      ++exponent;
    }
    if (exponent != 0) result.emplace_back(d, exponent);
  }
  if (n > 1) result.emplace_back(n, 1);
  return result;
}

void TestSmallSieves() {
  for (int limit = 0; limit <= 200; ++limit) {
    const nt::Sieve sieve(limit);
    Require(sieve.spf.size() == static_cast<std::size_t>(limit) + 1,
            "Wrong sieve size");
    Require(sieve.phi.size() == sieve.spf.size() &&
                sieve.mu.size() == sieve.spf.size(), "Wrong function sizes");
    std::vector<int> primes;
    for (int n = 1; n <= limit; ++n) {
      const auto factors = TrialFactor(n);
      Require(sieve.Factorize(n) == factors, "Wrong SPF factorization");
      const int smallest = n == 1 ? 0 : static_cast<int>(factors.front().first);
      Require(sieve.spf[n] == smallest, "Wrong smallest prime factor");
      if (TrialPrime(n)) primes.push_back(n);
      int coprime = 0;
      for (int k = 1; k <= n; ++k) coprime += std::gcd(k, n) == 1;
      Require(sieve.phi[n] == coprime, "Totient disagrees with brute force");
      int mobius = factors.size() % 2 == 0 ? 1 : -1;
      for (auto [prime, exponent] : factors) {
        if (exponent > 1) mobius = 0;
      }
      Require(sieve.mu[n] == mobius, "Wrong Mobius value");
    }
    Require(sieve.primes == primes, "Wrong prime list");
  }
}

void TestDivisorIdentities() {
  constexpr int kLimit = 200000;
  const nt::Sieve sieve(kLimit);
  std::vector<int> phi_sum(kLimit + 1), mu_sum(kLimit + 1);
  for (int d = 1; d <= kLimit; ++d) {
    for (int n = d; n <= kLimit; n += d) {
      phi_sum[n] += sieve.phi[d];
      mu_sum[n] += sieve.mu[d];
    }
  }
  for (int n = 1; n <= kLimit; ++n) {
    Require(phi_sum[n] == n, "Sum of totients over divisors is not n");
    Require(mu_sum[n] == (n == 1), "Mobius divisor identity failed");
    Require(sieve.Factorize(n) == TrialFactor(n), "Larger factorization failed");
  }
}

void CheckSegment(std::int64_t left, std::int64_t right) {
  std::vector<std::int64_t> expected;
  for (auto n = left; n <= right; ++n) {
    if (TrialPrime(n)) expected.push_back(n);
  }
  Require(nt::SegmentedPrimes(left, right) == expected,
          "Segmented sieve disagrees with trial division");
}

void TestSegments() {
  for (int left = 0; left <= 100; ++left) {
    for (int right = left; right <= 100; ++right) CheckSegment(left, right);
  }
  for (std::int64_t p : {2, 3, 7, 97, 997, 1009, 10007, 999983}) {
    const auto square = p * p;
    CheckSegment(square - 1, square - 1);
    CheckSegment(square, square);
    CheckSegment(square + 1, square + 1);
    CheckSegment(square - 3, square + 3);
  }
  for (std::int64_t left : {1000000LL, 1000000000LL, 1000000000000LL}) {
    CheckSegment(left, left + 100);
  }
  const nt::Sieve sieve(100000);
  const auto actual = nt::SegmentedPrimes(0, 100000);
  const std::vector<std::int64_t> expected(sieve.primes.begin(), sieve.primes.end());
  Require(actual == expected, "Full segment disagrees with linear sieve");
}

}  // namespace

int main() {
  try {
    TestSmallSieves();
    TestDivisorIdentities();
    TestSegments();
  } catch (const std::exception& error) {
    std::cerr << "Sieve test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "Sieve tests passed\n";
}
