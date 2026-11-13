#include "factorization.h"
#include <iostream>

int main() {
  const auto factors = nt::Factorize(360);
  for (auto [p, e] : factors) std::cout << p << '^' << e << ' ';
  std::cout << '\n';  // 2^3 3^2 5^1
  std::cout << nt::Totient(factors) << ' ' << nt::DivisorCount(factors)
            << ' ' << nt::DivisorSum(factors) << '\n';  // 96 24 1170
}
