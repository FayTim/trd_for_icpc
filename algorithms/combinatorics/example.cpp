#include "binomial.h"

#include <iostream>

int main() {
  const auto exact = combinatorics::Binomial(67, 33);
  if (exact) std::cout << *exact << '\n';  // 14226520737620288370.
  if (!combinatorics::Binomial(68, 34))
    std::cout << "Does not fit uint64_t\n";

  const combinatorics::PrimeBinomial table(1000, 1'000'000'007);
  std::cout << table.Choose(5, 2) << '\n';   // 10.
  std::cout << table.Choose(5, 7) << '\n';   // 0.
}
