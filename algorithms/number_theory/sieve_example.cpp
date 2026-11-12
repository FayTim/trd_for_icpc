#include "sieve.h"

#include <iostream>

int main() {
  const nt::Sieve sieve(100);
  std::cout << sieve.phi[12] << ' ' << sieve.mu[30] << '\n';  // 4 -1
  for (auto [prime, exponent] : sieve.Factorize(72)) {
    std::cout << prime << '^' << exponent << ' ';  // 2^3 3^2
  }
  std::cout << '\n';
  for (auto prime : nt::SegmentedPrimes(100, 120)) {
    std::cout << prime << ' ';  // 101 103 107 109 113
  }
  std::cout << '\n';
}
